#include "particle/VulkanParticlePipeline.hpp"

#include <stdexcept>
#include <random>

#include "utils/VulkanShader.hpp"
#include "utils/VulkanMemory.hpp"
#include "utils/VulkanPhysicalDevice.hpp"
#include "utils/VulkanCommandBuffer.hpp"
#include "utils/VulkanDescriptorUtils.hpp"
#include "ubo/VulkanUboStructs.hpp"

void
VulkanParticlePipeline::init(VulkanInstance const &vkInstance,
                             VulkanSwapChain const &swapChain,
                             uint32_t nbParticles,
                             glm::vec3 const &particles_color,
                             VkBuffer systemUbo)
{
    _devices = vkInstance.devices;
    _cmdPools = vkInstance.cmdPools;
    _queues = vkInstance.queues;

    // Global
    _gfxUbo.color = particles_color;
    _pipelineData.init(_devices, nbParticles);
    _renderPass.init(vkInstance, swapChain);
    createDescriptorPool(swapChain.currentSwapChainNbImg);

    // Vertex / Fragment shaders related
    _gfxDescription.init(_devices);
    _gfxUniform.allocate(_devices,
                         sizeof(ParticleGfxUbo) *
                           swapChain.currentSwapChainNbImg,
                         VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    createGfxPipeline(swapChain);
    createGfxDescriptorSets(systemUbo, swapChain.currentSwapChainNbImg);
    generateParticlesOnCpu();

    // Compute shaders related
    _computeUniform.allocate(_devices,
                             sizeof(ParticleComputeUbo),
                             VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    ParticleComputeUbo ubo{ _pipelineData.nbParticles };
    copyOnCpuCoherentMemory(_devices.device,
                            _computeUniform.memory,
                            0,
                            sizeof(ParticleComputeUbo),
                            &ubo);
    _computeDescription.init(_devices);
    createComputePipeline();
    createComputeDescriptorSets();
}

void
VulkanParticlePipeline::resize(VulkanSwapChain const &swapChain,
                               VkBuffer systemUbo)
{
    (void)swapChain;
    (void)systemUbo;
/*    vkDestroyBuffer(_devices.device, _particle_uniform, nullptr);
    vkFreeMemory(_devices.device, _particle_uniform_memory, nullptr);
    vkDestroyPipeline(_devices.device, _gfxPipeline, nullptr);
    vkDestroyPipelineLayout(_devices.device, _pipeline_layout, nullptr);
    _renderPass.resize(swapChain);

    _create_particle_debug_uniform_buffer(swapChain.currentSwapChainNbImg);
    _create_pipeline_layout();
    createGfxPipeline(swapChain);
    vkDestroyDescriptorPool(
      _devices.device, _pipelineData.descriptorPool, nullptr);
    createDescriptorPool(swapChain, _pipelineData);
    createGfxDescriptorSets(swapChain, _pipelineData, systemUbo);
    createComputeDescriptorSets(_pipelineData);*/
}

void
VulkanParticlePipeline::clear()
{
/*    vkDestroyBuffer(_devices.device, _particle_compute_uniform, nullptr);
    vkFreeMemory(_devices.device, _particle_compute_uniform_memory, nullptr);
    vkDestroyBuffer(_devices.device, _particle_uniform, nullptr);
    vkFreeMemory(_devices.device, _particle_uniform_memory, nullptr);
    vkDestroyPipeline(_devices.device, _gfxPipeline, nullptr);
    vkDestroyPipelineLayout(_devices.device, _pipeline_layout, nullptr);
    _renderPass.clear();
    vkDestroyDescriptorSetLayout(
      _devices.device, _descriptor_set_layout, nullptr);
    vkDestroyBuffer(_devices.device, _pipelineData.buffer, nullptr);
    vkFreeMemory(_devices.device, _pipelineData.memory, nullptr);
    vkDestroyDescriptorPool(
      _devices.device, _pipelineData.descriptorPool, nullptr);

    vkDestroyPipeline(_devices.device, _compMoveForwardPipeline, nullptr);
    vkDestroyPipelineLayout(_devices.device, _compute_pipeline_layout, nullptr);
    vkDestroyDescriptorSetLayout(
      _devices.device, _compute_descriptor_set_layout, nullptr);

    _devices = VulkanDevices{};
    _queues = VulkanQueues{};
    _cmdPools = VulkanCommandPools{};
    _descriptor_set_layout = nullptr;
    _pipeline_layout = nullptr;
    _gfxPipeline = nullptr;
    _pipelineData.clear();*/
}

void
VulkanParticlePipeline::setParticleNumber(uint32_t nbParticles,
                                          VulkanSwapChain const &swapChain,
                                          VkBuffer systemUbo)
{
    vkDestroyDescriptorPool(
      _devices.device, _descriptorPool, nullptr);
    _pipelineData.data.clear();

    _pipelineData.init(_devices, nbParticles);
    generateParticlesOnCpu();
    createDescriptorPool(swapChain.currentSwapChainNbImg);
    createGfxDescriptorSets(systemUbo, swapChain.currentSwapChainNbImg);
    createComputeDescriptorSets();
    ParticleComputeUbo ubo{ _pipelineData.nbParticles };
    copyOnCpuCoherentMemory(_devices.device,
                            _computeUniform.memory,
                            0,
                            sizeof(ParticleComputeUbo),
                            &ubo);
}

void
VulkanParticlePipeline::setParticlesColor(glm::vec3 const &particlesColor)
{
    _gfxUbo.color = particlesColor;
}

void
VulkanParticlePipeline::setParticleGravityCenter(
  glm::vec3 const &particleGravityCenter)
{
    _gfxUbo.center = particleGravityCenter;
}

void
VulkanParticlePipeline::setUniformOnGpu(uint32_t currentImg)
{
    copyOnCpuCoherentMemory(_devices.device,
                            _gfxUniform.memory,
                            currentImg * sizeof(ParticleGfxUbo),
                            sizeof(ParticleGfxUbo),
                            &_gfxUbo);
}

VulkanParticleRenderPass const &
VulkanParticlePipeline::getRenderPass() const
{
    return (_renderPass);
}

void
VulkanParticlePipeline::generateCommands(VkCommandBuffer cmdBuffer,
                                         size_t descriptorSetIndex)
{
    // Vertex related values
    VkBuffer vertex_buffer[] = { _pipelineData.data.buffer };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _gfxPipeline);
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertex_buffer, offsets);

    vkCmdBindDescriptorSets(cmdBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            _gfxDescription.pipelineLayout,
                            0,
                            1,
                            &_gfxDescriptorSets[descriptorSetIndex],
                            0,
                            nullptr);
    vkCmdDraw(cmdBuffer, _pipelineData.nbParticles, 1, 0, 0);
}

void
VulkanParticlePipeline::generateComputeCommands(VkCommandBuffer cmdBuffer)
{
    vkCmdBindPipeline(
      cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, _compMoveForwardPipeline);
    vkCmdBindDescriptorSets(cmdBuffer,
                            VK_PIPELINE_BIND_POINT_COMPUTE,
                            _computeDescription.pipelineLayout,
                            0,
                            1,
                            &_computeDescriptorSet[0],
                            0,
                            nullptr);
    vkCmdDispatch(
      cmdBuffer,
      (_pipelineData.nbParticles / _computeDescription.workGroupSize) + 1,
      1,
      1);
}

void
VulkanParticlePipeline::createGfxPipeline(VulkanSwapChain const &swapChain)
{
    // Shaders
    auto vert_shader = loadShader(
      _devices.device, "resources/shaders/particle/particle.vert.spv");
    auto frag_shader = loadShader(
      _devices.device, "resources/shaders/particle/particle.frag.spv");

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
    auto const &binding_description =
      VulkanParticleGfxPipelineDescription::inputBindingDescription;
    auto const &attribute_description =
      VulkanParticleGfxPipelineDescription::inputAttributeDescription;
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
    gfx_pipeline_info.layout = _gfxDescription.pipelineLayout;
    gfx_pipeline_info.renderPass = _renderPass.renderPass;
    gfx_pipeline_info.subpass = 0;
    gfx_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    gfx_pipeline_info.basePipelineIndex = -1;
    if (vkCreateGraphicsPipelines(_devices.device,
                                  VK_NULL_HANDLE,
                                  1,
                                  &gfx_pipeline_info,
                                  nullptr,
                                  &_gfxPipeline) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanParticlePipeline: Failed to create graphic pipeline");
    }

    vkDestroyShaderModule(_devices.device, vert_shader, nullptr);
    vkDestroyShaderModule(_devices.device, frag_shader, nullptr);
}

void
VulkanParticlePipeline::createDescriptorPool(uint32_t descriptorCount)
{
    std::array<VkDescriptorPoolSize, 4> const poolSize{ {
      { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptorCount },
      { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptorCount },
      { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1 },
      { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 },
    } };

    _descriptorPool =
      ::createDescriptorPool(_devices, poolSize, descriptorCount + 1);
}

void
VulkanParticlePipeline::createGfxDescriptorSets(VkBuffer systemUbo,
                                             uint32_t descriptorCount)
{
    uint32_t totalDescriptorCount = descriptorCount + 1;

    allocateDescriptorSets(_devices,
                           _descriptorPool,
                           _gfxDescription.descriptorSetLayout,
                           totalDescriptorCount,
                           _gfxDescriptorSets);

    for (size_t i = 0; i < totalDescriptorCount; ++i) {
        std::array<VkWriteDescriptorSet, 2> descriptor_write{};

        // System UBO
        VkDescriptorBufferInfo system_buffer_info{};
        system_buffer_info.buffer = systemUbo;
        system_buffer_info.offset = 0;
        system_buffer_info.range = sizeof(SystemUbo);
        descriptor_write[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write[0].dstSet = _gfxDescriptorSets[i];
        descriptor_write[0].dstBinding = 0;
        descriptor_write[0].dstArrayElement = 0;
        descriptor_write[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_write[0].descriptorCount = 1;
        descriptor_write[0].pBufferInfo = &system_buffer_info;
        descriptor_write[0].pImageInfo = nullptr;
        descriptor_write[0].pTexelBufferView = nullptr;

        // ParticleDebug UBO
        VkDescriptorBufferInfo particle_debug_buffer_info{};
        particle_debug_buffer_info.buffer = _gfxUniform.buffer;
        particle_debug_buffer_info.offset = 0;
        particle_debug_buffer_info.range = sizeof(ParticleGfxUbo);
        descriptor_write[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write[1].dstSet = _gfxDescriptorSets[i];
        descriptor_write[1].dstBinding = 1;
        descriptor_write[1].dstArrayElement = 0;
        descriptor_write[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_write[1].descriptorCount = 1;
        descriptor_write[1].pBufferInfo = &particle_debug_buffer_info;
        descriptor_write[1].pImageInfo = nullptr;
        descriptor_write[1].pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(_devices.device,
                               descriptor_write.size(),
                               descriptor_write.data(),
                               0,
                               nullptr);
    }
}

void
VulkanParticlePipeline::generateParticlesOnCpu() const
{
    std::vector<VulkanParticle> particles(_pipelineData.nbParticles);

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_real_distribution<float> dist(-5.0f, 5.0f);

    for (uint64_t i = 0; i < _pipelineData.nbParticles; ++i) {
        particles[i].position = glm::vec3(dist(gen), dist(gen), dist(gen));
    }

    VkDeviceSize buff_size = sizeof(glm::vec3) * _pipelineData.nbParticles;
    copyCpuBufferToGpu(_devices.device,
                       _devices.physicalDevice,
                       _cmdPools.renderCommandPool,
                       _queues.graphicQueue,
                       _pipelineData.data.buffer,
                       &particles[0],
                       { 0, 0, buff_size });
}

void
VulkanParticlePipeline::createComputePipeline()
{
    // Shaders
    auto comp_shader =
      loadShader(_devices.device,
                 "resources/shaders/particle/particleMoveForward.comp.spv");

    VkPipelineShaderStageCreateInfo comp_shader_info{};
    comp_shader_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    comp_shader_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    comp_shader_info.module = comp_shader;
    comp_shader_info.pName = "main";

    // Compute pipeline specialization info
    std::array<VkSpecializationMapEntry, 1> speData{};
    speData[0].constantID = 0;
    speData[0].offset = 0;
    speData[0].size = sizeof(uint32_t);

    VkSpecializationInfo speInfo{};
    speInfo.dataSize = sizeof(uint32_t);
    speInfo.mapEntryCount = speData.size();
    speInfo.pMapEntries = speData.data();
    speInfo.pData = &_computeDescription.workGroupSize;

    // Compute pipeline creation
    VkComputePipelineCreateInfo comp_pipeline_info{};
    comp_pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    comp_pipeline_info.layout = _computeDescription.pipelineLayout;
    comp_pipeline_info.flags = 0;
    comp_pipeline_info.stage = comp_shader_info;
    comp_pipeline_info.stage.pSpecializationInfo = &speInfo;

    if (vkCreateComputePipelines(_devices.device,
                                 VK_NULL_HANDLE,
                                 1,
                                 &comp_pipeline_info,
                                 nullptr,
                                 &_compMoveForwardPipeline) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanParticlePipeline: Failed to create compute pipeline");
    }

    vkDestroyShaderModule(_devices.device, comp_shader, nullptr);
}

void
VulkanParticlePipeline::createComputeDescriptorSets()
{
    allocateDescriptorSets(_devices,
                           _descriptorPool,
                           _computeDescription.descriptorSetLayout,
                           1,
                           _computeDescriptorSet);

    std::array<VkWriteDescriptorSet, 2> descriptor_write{};

    // Storage buffer
    VkDescriptorBufferInfo storage_buffer_info{};
    storage_buffer_info.buffer = _pipelineData.data.buffer;
    storage_buffer_info.offset = 0;
    storage_buffer_info.range =
      sizeof(VulkanParticle) * _pipelineData.nbParticles;
    descriptor_write[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write[0].dstSet = _computeDescriptorSet[0];
    descriptor_write[0].dstBinding = 0;
    descriptor_write[0].dstArrayElement = 0;
    descriptor_write[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptor_write[0].descriptorCount = 1;
    descriptor_write[0].pBufferInfo = &storage_buffer_info;
    descriptor_write[0].pImageInfo = nullptr;
    descriptor_write[0].pTexelBufferView = nullptr;

    // ParticleComputeDebug UBO
    VkDescriptorBufferInfo particle_compute_ubo_buffer_info{};
    particle_compute_ubo_buffer_info.buffer = _computeUniform.buffer;
    particle_compute_ubo_buffer_info.offset = 0;
    particle_compute_ubo_buffer_info.range = sizeof(ParticleComputeUbo);
    descriptor_write[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write[1].dstSet = _computeDescriptorSet[0];
    descriptor_write[1].dstBinding = 1;
    descriptor_write[1].dstArrayElement = 0;
    descriptor_write[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_write[1].descriptorCount = 1;
    descriptor_write[1].pBufferInfo = &particle_compute_ubo_buffer_info;
    descriptor_write[1].pImageInfo = nullptr;
    descriptor_write[1].pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(_devices.device,
                           descriptor_write.size(),
                           descriptor_write.data(),
                           0,
                           nullptr);
}
