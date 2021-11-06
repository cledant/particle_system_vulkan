#include "particleDisplayDebug/VulkanParticleDebugPipeline.hpp"

#include <stdexcept>
#include <random>

#include "utils/VulkanShader.hpp"
#include "utils/VulkanMemory.hpp"
#include "utils/VulkanPhysicalDevice.hpp"
#include "utils/VulkanCommandBuffer.hpp"
#include "VulkanUboStructs.hpp"

void
VulkanParticleDebugPipeline::init(VulkanInstance const &vkInstance,
                                  VulkanSwapChain const &swapChain,
                                  uint64_t nbParticles,
                                  glm::vec3 const &particles_color,
                                  VkBuffer systemUbo)
{
    _device = vkInstance.device;
    _physical_device = vkInstance.physicalDevice;
    _cmd_pool = vkInstance.renderCommandPool;
    _gfx_queue = vkInstance.graphicQueue;
    _pipeline_render_pass.init(vkInstance, swapChain);
    _create_particle_debug_uniform_buffer(swapChain.currentSwapChainNbImg);
    _create_descriptor_layout();
    _create_pipeline_layout();
    _create_gfx_pipeline(swapChain);
    _pipeline_data = _create_pipeline_particle_debug(nbParticles);
    _create_descriptor_pool(swapChain, _pipeline_data);
    _create_descriptor_sets(swapChain, _pipeline_data, systemUbo);
    _generate_particles();
    _particles_color = particles_color;

    _compute_cmd_pool = vkInstance.computeCommandPool;
    _compute_queue = vkInstance.computeQueue;
    _create_particle_compute_debug_uniform_buffer();
    _create_compute_descriptor_layout();
    _create_compute_descriptor_sets(_pipeline_data);
    _create_compute_pipeline_layout();
    _create_compute_pipeline();
    ParticleComputeDebugUbo ubo{ static_cast<int32_t>(
      _pipeline_data.nbParticles) };
    copyOnCpuCoherentMemory(_device,
                            _particle_compute_uniform_memory,
                            0,
                            sizeof(ParticleComputeDebugUbo),
                            &ubo);
}

void
VulkanParticleDebugPipeline::resize(VulkanSwapChain const &swapChain,
                                    VkBuffer systemUbo)
{
    vkDestroyBuffer(_device, _particle_uniform, nullptr);
    vkFreeMemory(_device, _particle_uniform_memory, nullptr);
    vkDestroyPipeline(_device, _graphic_pipeline, nullptr);
    vkDestroyPipelineLayout(_device, _pipeline_layout, nullptr);
    _pipeline_render_pass.resize(swapChain);

    _create_particle_debug_uniform_buffer(swapChain.currentSwapChainNbImg);
    _create_pipeline_layout();
    _create_gfx_pipeline(swapChain);
    vkDestroyDescriptorPool(_device, _pipeline_data.descriptorPool, nullptr);
    _create_descriptor_pool(swapChain, _pipeline_data);
    _create_descriptor_sets(swapChain, _pipeline_data, systemUbo);
    _create_compute_descriptor_sets(_pipeline_data);
}

void
VulkanParticleDebugPipeline::clear()
{
    vkDestroyBuffer(_device, _particle_compute_uniform, nullptr);
    vkFreeMemory(_device, _particle_compute_uniform_memory, nullptr);
    vkDestroyBuffer(_device, _particle_uniform, nullptr);
    vkFreeMemory(_device, _particle_uniform_memory, nullptr);
    vkDestroyPipeline(_device, _graphic_pipeline, nullptr);
    vkDestroyPipelineLayout(_device, _pipeline_layout, nullptr);
    _pipeline_render_pass.clear();
    vkDestroyDescriptorSetLayout(_device, _descriptor_set_layout, nullptr);
    vkDestroyBuffer(_device, _pipeline_data.buffer, nullptr);
    vkFreeMemory(_device, _pipeline_data.memory, nullptr);
    vkDestroyDescriptorPool(_device, _pipeline_data.descriptorPool, nullptr);

    vkDestroyPipeline(_device, _compute_pipeline, nullptr);
    vkDestroyPipelineLayout(_device, _compute_pipeline_layout, nullptr);
    vkDestroyDescriptorSetLayout(
      _device, _compute_descriptor_set_layout, nullptr);

    _device = nullptr;
    _physical_device = nullptr;
    _cmd_pool = nullptr;
    _gfx_queue = nullptr;
    _descriptor_set_layout = nullptr;
    _pipeline_layout = nullptr;
    _graphic_pipeline = nullptr;
    _pipeline_data.clear();

    _compute_cmd_pool = nullptr;
    _compute_queue = nullptr;
}

void
VulkanParticleDebugPipeline::setParticleNumber(uint64_t nbParticles,
                                               VulkanSwapChain const &swapChain,
                                               VkBuffer systemUbo)
{
    vkDestroyDescriptorPool(_device, _pipeline_data.descriptorPool, nullptr);

    _reallocate_pipeline_particle_debug_buffers(nbParticles);
    _create_descriptor_pool(swapChain, _pipeline_data);
    _create_descriptor_sets(swapChain, _pipeline_data, systemUbo);
    _create_compute_descriptor_sets(_pipeline_data);
    _generate_particles();
    ParticleComputeDebugUbo ubo{ static_cast<int32_t>(
      _pipeline_data.nbParticles) };
    copyOnCpuCoherentMemory(_device,
                            _particle_compute_uniform_memory,
                            0,
                            sizeof(ParticleComputeDebugUbo),
                            &ubo);
}

void
VulkanParticleDebugPipeline::setParticlesColor(glm::vec3 const &particlesColor)
{
    _particles_color = particlesColor;
}

void
VulkanParticleDebugPipeline::setParticleGravityCenter(
  glm::vec3 const &particleGravityCenter)
{
    _particles_gravity_center = particleGravityCenter;
}

void
VulkanParticleDebugPipeline::setUniformOnGpu(uint32_t currentImg)
{
    ParticleDebugUbo ubo{ _particles_gravity_center, _particles_color };

    copyOnCpuCoherentMemory(_device,
                            _particle_uniform_memory,
                            currentImg * sizeof(ParticleDebugUbo),
                            sizeof(ParticleDebugUbo),
                            &ubo);
}

VulkanParticleDebugRenderPass const &
VulkanParticleDebugPipeline::getRenderPass() const
{
    return (_pipeline_render_pass);
}

void
VulkanParticleDebugPipeline::generateCommands(VkCommandBuffer cmdBuffer,
                                              size_t descriptorSetIndex)
{
    // Vertex related values
    VkBuffer vertex_buffer[] = { _pipeline_data.buffer };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindPipeline(
      cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphic_pipeline);
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertex_buffer, offsets);

    vkCmdBindDescriptorSets(cmdBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            _pipeline_layout,
                            0,
                            1,
                            &_pipeline_data.descriptorSets[descriptorSetIndex],
                            0,
                            nullptr);
    vkCmdDraw(cmdBuffer, _pipeline_data.nbParticles, 1, 0, 0);
}

void
VulkanParticleDebugPipeline::generateComputeCommands(VkCommandBuffer cmdBuffer)
{
    vkCmdBindPipeline(
      cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, _compute_pipeline);
    vkCmdBindDescriptorSets(cmdBuffer,
                            VK_PIPELINE_BIND_POINT_COMPUTE,
                            _compute_pipeline_layout,
                            0,
                            1,
                            &_pipeline_data.computeDescriptorSet,
                            0,
                            nullptr);
    vkCmdDispatch(cmdBuffer,
                  (_pipeline_data.nbParticles % 256)
                    ? (_pipeline_data.nbParticles / 256) + 1
                    : _pipeline_data.nbParticles / 256,
                  1,
                  1);
}

void
VulkanParticleDebugPipeline::_create_descriptor_layout()
{
    VkDescriptorSetLayoutBinding system_ubo_layout_binding{};
    system_ubo_layout_binding.binding = 0;
    system_ubo_layout_binding.descriptorType =
      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    system_ubo_layout_binding.descriptorCount = 1;
    system_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    system_ubo_layout_binding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding particle_debug_layout_binding{};
    particle_debug_layout_binding.binding = 1;
    particle_debug_layout_binding.descriptorType =
      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    particle_debug_layout_binding.descriptorCount = 1;
    particle_debug_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    particle_debug_layout_binding.pImmutableSamplers = nullptr;

    std::array bindings{ system_ubo_layout_binding,
                         particle_debug_layout_binding };

    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = bindings.size();
    layout_info.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(
          _device, &layout_info, nullptr, &_descriptor_set_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error("VulkanParticleDebugPipeline: failed to "
                                 "create descriptor set layout");
    }
}

void
VulkanParticleDebugPipeline::_create_pipeline_layout()
{
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &_descriptor_set_layout;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(
          _device, &pipeline_layout_info, nullptr, &_pipeline_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanParticleDebugPipeline: Failed to create pipeline layout");
    }
}

void
VulkanParticleDebugPipeline::_create_gfx_pipeline(
  VulkanSwapChain const &swapChain)
{
    // Shaders
    auto vert_shader = loadShader(
      _device, "resources/shaders/particleDebug/particleDebug.vert.spv");
    auto frag_shader = loadShader(
      _device, "resources/shaders/particleDebug/particleDebug.frag.spv");

    VkPipelineShaderStageCreateInfo vert_shader_info{};
    vert_shader_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_shader_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_info.module = vert_shader;
    vert_shader_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_shader_info{};
    frag_shader_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_shader_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_info.module = frag_shader;
    frag_shader_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_stage_info[] = { vert_shader_info,
                                                            frag_shader_info };

    // Vertex input
    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    auto binding_description =
      VulkanParticleDebugPipelineData::getInputBindingDescription();
    auto attribute_description =
      VulkanParticleDebugPipelineData::getInputAttributeDescription();
    vertex_input_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount =
      binding_description.size();
    vertex_input_info.pVertexBindingDescriptions = binding_description.data();
    vertex_input_info.vertexAttributeDescriptionCount =
      attribute_description.size();
    vertex_input_info.pVertexAttributeDescriptions =
      attribute_description.data();

    // Input Assembly
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info{};
    input_assembly_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_info.primitiveRestartEnable = VK_FALSE;

    // Viewport
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = static_cast<float>(swapChain.swapChainExtent.height);
    viewport.height = -static_cast<float>(swapChain.swapChainExtent.height);
    viewport.width = swapChain.swapChainExtent.width;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChain.swapChainExtent;

    VkPipelineViewportStateCreateInfo viewport_state_info{};
    viewport_state_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_info.scissorCount = 1;
    viewport_state_info.pScissors = &scissor;
    viewport_state_info.viewportCount = 1;
    viewport_state_info.pViewports = &viewport;

    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer_info{};
    rasterizer_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer_info.depthBiasClamp = VK_FALSE;
    rasterizer_info.rasterizerDiscardEnable = VK_FALSE;
    rasterizer_info.polygonMode = VK_POLYGON_MODE_POINT;
    rasterizer_info.lineWidth = 1.0f;
    rasterizer_info.cullMode = VK_CULL_MODE_NONE;
    rasterizer_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer_info.depthBiasEnable = VK_FALSE;
    rasterizer_info.depthBiasConstantFactor = 0.0f;
    rasterizer_info.depthBiasClamp = 0.0f;
    rasterizer_info.depthBiasSlopeFactor = 0.0f;

    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampling_info{};
    multisampling_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling_info.sampleShadingEnable = VK_FALSE;
    multisampling_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling_info.minSampleShading = 1.0f;
    multisampling_info.pSampleMask = nullptr;
    multisampling_info.alphaToCoverageEnable = VK_FALSE;
    multisampling_info.alphaToOneEnable = VK_FALSE;

    // Color Blending
    VkPipelineColorBlendAttachmentState color_blending_attachment{};
    color_blending_attachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blending_attachment.blendEnable = VK_FALSE;
    color_blending_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blending_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blending_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blending_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blending_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blending_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blending_info{};
    color_blending_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending_info.logicOpEnable = VK_FALSE;
    color_blending_info.attachmentCount = 1;
    color_blending_info.pAttachments = &color_blending_attachment;
    color_blending_info.logicOp = VK_LOGIC_OP_COPY;
    color_blending_info.blendConstants[0] = 0.0f;
    color_blending_info.blendConstants[1] = 0.0f;
    color_blending_info.blendConstants[2] = 0.0f;
    color_blending_info.blendConstants[3] = 0.0f;

    // Depth
    VkPipelineDepthStencilStateCreateInfo depth_stencil{};
    depth_stencil.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.minDepthBounds = 0.0f;
    depth_stencil.maxDepthBounds = 1.0f;
    depth_stencil.stencilTestEnable = VK_FALSE;
    depth_stencil.front = {};
    depth_stencil.back = {};

    // Gfx pipeline creation
    VkGraphicsPipelineCreateInfo gfx_pipeline_info{};
    gfx_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    gfx_pipeline_info.stageCount = 2;
    gfx_pipeline_info.pStages = shader_stage_info;
    gfx_pipeline_info.pVertexInputState = &vertex_input_info;
    gfx_pipeline_info.pInputAssemblyState = &input_assembly_info;
    gfx_pipeline_info.pViewportState = &viewport_state_info;
    gfx_pipeline_info.pRasterizationState = &rasterizer_info;
    gfx_pipeline_info.pMultisampleState = &multisampling_info;
    gfx_pipeline_info.pDepthStencilState = &depth_stencil;
    gfx_pipeline_info.pColorBlendState = &color_blending_info;
    gfx_pipeline_info.pDynamicState = nullptr;
    gfx_pipeline_info.layout = _pipeline_layout;
    gfx_pipeline_info.renderPass = _pipeline_render_pass.renderPass;
    gfx_pipeline_info.subpass = 0;
    gfx_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    gfx_pipeline_info.basePipelineIndex = -1;
    if (vkCreateGraphicsPipelines(_device,
                                  VK_NULL_HANDLE,
                                  1,
                                  &gfx_pipeline_info,
                                  nullptr,
                                  &_graphic_pipeline) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanParticleDebugPipeline: Failed to create graphic pipeline");
    }

    vkDestroyShaderModule(_device, vert_shader, nullptr);
    vkDestroyShaderModule(_device, frag_shader, nullptr);
}

VulkanParticleDebugPipelineData
VulkanParticleDebugPipeline::_create_pipeline_particle_debug(
  uint64_t nbParticles)
{
    VulkanParticleDebugPipelineData pipeline_particle{};

    // Computing sizes and offsets
    pipeline_particle.nbParticles = nbParticles;
    pipeline_particle.particleBufferSize =
      sizeof(VulkanParticleDebug) * nbParticles;
    VkDeviceSize total_size = pipeline_particle.particleBufferSize;

    // Creating GPU buffer
    createBuffer(_device,
                 pipeline_particle.buffer,
                 total_size,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                   VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    allocateBuffer(_physical_device,
                   _device,
                   pipeline_particle.buffer,
                   pipeline_particle.memory,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    return (pipeline_particle);
}

void
VulkanParticleDebugPipeline::_reallocate_pipeline_particle_debug_buffers(
  uint64_t nbParticles)
{
    // Remove previous buffers
    vkDestroyBuffer(_device, _pipeline_data.buffer, nullptr);
    vkFreeMemory(_device, _pipeline_data.memory, nullptr);

    // Computing sizes and offsets
    _pipeline_data.nbParticles = nbParticles;
    _pipeline_data.particleBufferSize =
      sizeof(VulkanParticleDebug) * nbParticles;
    VkDeviceSize total_size = _pipeline_data.particleBufferSize;

    // Creating GPU buffer
    createBuffer(_device,
                 _pipeline_data.buffer,
                 total_size,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                   VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    allocateBuffer(_physical_device,
                   _device,
                   _pipeline_data.buffer,
                   _pipeline_data.memory,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

void
VulkanParticleDebugPipeline::_create_descriptor_pool(
  VulkanSwapChain const &swapChain,
  VulkanParticleDebugPipelineData &pipelineData)
{
    std::array<VkDescriptorPoolSize, 4> pool_size{};
    // System Ubo
    pool_size[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_size[0].descriptorCount = swapChain.currentSwapChainNbImg;
    // ParticleDebug Ubo
    pool_size[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_size[1].descriptorCount = swapChain.currentSwapChainNbImg;
    // Storage buffer
    pool_size[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    pool_size[2].descriptorCount = 1;
    // ParticleDebug Compute Ubo
    pool_size[3].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_size[3].descriptorCount = 1;

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = pool_size.size();
    pool_info.pPoolSizes = pool_size.data();
    pool_info.maxSets = swapChain.currentSwapChainNbImg + 1;

    if (vkCreateDescriptorPool(
          _device, &pool_info, nullptr, &pipelineData.descriptorPool) !=
        VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanParticleDebugPipeline: failed to create descriptor pool");
    }
}

void
VulkanParticleDebugPipeline::_create_descriptor_sets(
  VulkanSwapChain const &swapChain,
  VulkanParticleDebugPipelineData &pipelineData,
  VkBuffer systemUbo)
{
    std::vector<VkDescriptorSetLayout> layouts(swapChain.currentSwapChainNbImg,
                                               _descriptor_set_layout);

    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = pipelineData.descriptorPool;
    alloc_info.descriptorSetCount = swapChain.currentSwapChainNbImg;
    alloc_info.pSetLayouts = layouts.data();

    pipelineData.descriptorSets.resize(swapChain.currentSwapChainNbImg);
    if (vkAllocateDescriptorSets(
          _device, &alloc_info, pipelineData.descriptorSets.data()) !=
        VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanParticleDebugPipeline: failed to create descriptor sets");
    }

    for (size_t i = 0; i < swapChain.currentSwapChainNbImg; ++i) {
        std::array<VkWriteDescriptorSet, 2> descriptor_write{};

        // System UBO
        VkDescriptorBufferInfo system_buffer_info{};
        system_buffer_info.buffer = systemUbo;
        system_buffer_info.offset = 0;
        system_buffer_info.range = sizeof(SystemUbo);
        descriptor_write[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write[0].dstSet = pipelineData.descriptorSets[i];
        descriptor_write[0].dstBinding = 0;
        descriptor_write[0].dstArrayElement = 0;
        descriptor_write[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_write[0].descriptorCount = 1;
        descriptor_write[0].pBufferInfo = &system_buffer_info;
        descriptor_write[0].pImageInfo = nullptr;
        descriptor_write[0].pTexelBufferView = nullptr;

        // ParticleDebug UBO
        VkDescriptorBufferInfo particle_debug_buffer_info{};
        particle_debug_buffer_info.buffer = _particle_uniform;
        particle_debug_buffer_info.offset = 0;
        particle_debug_buffer_info.range = sizeof(ParticleDebugUbo);
        descriptor_write[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write[1].dstSet = pipelineData.descriptorSets[i];
        descriptor_write[1].dstBinding = 1;
        descriptor_write[1].dstArrayElement = 0;
        descriptor_write[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_write[1].descriptorCount = 1;
        descriptor_write[1].pBufferInfo = &particle_debug_buffer_info;
        descriptor_write[1].pImageInfo = nullptr;
        descriptor_write[1].pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(_device,
                               descriptor_write.size(),
                               descriptor_write.data(),
                               0,
                               nullptr);
    }
}

void
VulkanParticleDebugPipeline::_create_particle_debug_uniform_buffer(
  uint32_t currentSwapChainNbImg)
{
    createBuffer(_device,
                 _particle_uniform,
                 sizeof(ParticleDebugUbo) * currentSwapChainNbImg,
                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    allocateBuffer(_physical_device,
                   _device,
                   _particle_uniform,
                   _particle_uniform_memory,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void
VulkanParticleDebugPipeline::_generate_particles()
{
    std::vector<VulkanParticleDebug> particles(_pipeline_data.nbParticles);

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_real_distribution<float> dist(-5.0f, 5.0f);

    for (uint64_t i = 0; i < _pipeline_data.nbParticles; ++i) {
        particles[i].position = glm::vec3(dist(gen), dist(gen), dist(gen));
    }

    VkDeviceSize buff_size = sizeof(glm::vec3) * _pipeline_data.nbParticles;
    copyCpuBufferToGpu(_device,
                       _physical_device,
                       _cmd_pool,
                       _gfx_queue,
                       _pipeline_data.buffer,
                       &particles[0],
                       { 0, 0, buff_size });
}

void
VulkanParticleDebugPipeline::_create_particle_compute_debug_uniform_buffer()
{

    createBuffer(_device,
                 _particle_compute_uniform,
                 sizeof(ParticleComputeDebugUbo),
                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    allocateBuffer(_physical_device,
                   _device,
                   _particle_compute_uniform,
                   _particle_compute_uniform_memory,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void
VulkanParticleDebugPipeline::_create_compute_descriptor_layout()
{
    VkDescriptorSetLayoutBinding storage_buffer_layout_binding{};
    storage_buffer_layout_binding.binding = 0;
    storage_buffer_layout_binding.descriptorType =
      VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    storage_buffer_layout_binding.descriptorCount = 1;
    storage_buffer_layout_binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    storage_buffer_layout_binding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding particle_debug_compute_layout_binding{};
    particle_debug_compute_layout_binding.binding = 1;
    particle_debug_compute_layout_binding.descriptorType =
      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    particle_debug_compute_layout_binding.descriptorCount = 1;
    particle_debug_compute_layout_binding.stageFlags =
      VK_SHADER_STAGE_COMPUTE_BIT;
    particle_debug_compute_layout_binding.pImmutableSamplers = nullptr;

    std::array bindings{ storage_buffer_layout_binding,
                         particle_debug_compute_layout_binding };

    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = bindings.size();
    layout_info.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(
          _device, &layout_info, nullptr, &_compute_descriptor_set_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error("VulkanParticleDebugPipeline: failed to "
                                 "create compute descriptor set layout");
    }
}

void
VulkanParticleDebugPipeline::_create_compute_pipeline_layout()
{
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &_compute_descriptor_set_layout;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(
          _device, &pipeline_layout_info, nullptr, &_compute_pipeline_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error("VulkanParticleDebugPipeline: Failed to "
                                 "create compute pipeline layout");
    }
}

void
VulkanParticleDebugPipeline::_create_compute_pipeline()
{
    // Shaders
    auto comp_shader = loadShader(
      _device, "resources/shaders/particleDebug/particleDebug.comp.spv");

    VkPipelineShaderStageCreateInfo comp_shader_info{};
    comp_shader_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    comp_shader_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    comp_shader_info.module = comp_shader;
    comp_shader_info.pName = "main";

    // Compute pipeline creation
    VkComputePipelineCreateInfo comp_pipeline_info{};
    comp_pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    comp_pipeline_info.layout = _compute_pipeline_layout;
    comp_pipeline_info.flags = 0;
    comp_pipeline_info.stage = comp_shader_info;

    if (vkCreateComputePipelines(_device,
                                 VK_NULL_HANDLE,
                                 1,
                                 &comp_pipeline_info,
                                 nullptr,
                                 &_compute_pipeline) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanParticleDebugPipeline: Failed to create compute pipeline");
    }

    vkDestroyShaderModule(_device, comp_shader, nullptr);
}

void
VulkanParticleDebugPipeline::_create_compute_descriptor_sets(
  VulkanParticleDebugPipelineData &pipelineData)
{
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = pipelineData.descriptorPool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &_compute_descriptor_set_layout;

    if (vkAllocateDescriptorSets(
          _device, &alloc_info, &_pipeline_data.computeDescriptorSet) !=
        VK_SUCCESS) {
        throw std::runtime_error("VulkanParticleDebugPipeline: failed to "
                                 "create compute descriptor sets");
    }

    std::array<VkWriteDescriptorSet, 2> descriptor_write{};

    // Storage buffer
    VkDescriptorBufferInfo storage_buffer_info{};
    storage_buffer_info.buffer = _pipeline_data.buffer;
    storage_buffer_info.offset = 0;
    storage_buffer_info.range =
      sizeof(VulkanParticleDebug) * pipelineData.nbParticles;
    descriptor_write[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write[0].dstSet = pipelineData.computeDescriptorSet;
    descriptor_write[0].dstBinding = 0;
    descriptor_write[0].dstArrayElement = 0;
    descriptor_write[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptor_write[0].descriptorCount = 1;
    descriptor_write[0].pBufferInfo = &storage_buffer_info;
    descriptor_write[0].pImageInfo = nullptr;
    descriptor_write[0].pTexelBufferView = nullptr;

    // ParticleComputeDebug UBO
    VkDescriptorBufferInfo particle_compute_ubo_buffer_info{};
    particle_compute_ubo_buffer_info.buffer = _particle_compute_uniform;
    particle_compute_ubo_buffer_info.offset = 0;
    particle_compute_ubo_buffer_info.range = sizeof(ParticleComputeDebugUbo);
    descriptor_write[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write[1].dstSet = pipelineData.computeDescriptorSet;
    descriptor_write[1].dstBinding = 1;
    descriptor_write[1].dstArrayElement = 0;
    descriptor_write[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_write[1].descriptorCount = 1;
    descriptor_write[1].pBufferInfo = &particle_compute_ubo_buffer_info;
    descriptor_write[1].pImageInfo = nullptr;
    descriptor_write[1].pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(
      _device, descriptor_write.size(), descriptor_write.data(), 0, nullptr);
}
