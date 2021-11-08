#include "skybox/VulkanSkyboxPipeline.hpp"

#include <stdexcept>

#include "skybox/SkyboxModel.hpp"
#include "utils/VulkanShader.hpp"
#include "utils/VulkanMemory.hpp"
#include "utils/VulkanPhysicalDevice.hpp"
#include "utils/VulkanCommandBuffer.hpp"
#include "VulkanUboStructs.hpp"

void
VulkanSkyboxPipeline::init(VulkanInstance const &vkInstance,
                           VulkanSwapChain const &swapChain,
                           std::string const &skyboxFolderPath,
                           std::string const &skyboxFileType,
                           VulkanTextureManager &texManager,
                           VkBuffer systemUbo)
{
    _devices = vkInstance.devices;
    _cmdPools = vkInstance.cmdPools;
    _queues = vkInstance.queues;
    _pipeline_render_pass.init(vkInstance, swapChain);
    _skybox_folder_path = skyboxFolderPath;
    _skybox_filetype = skyboxFileType;
    _skybox_tex =
      texManager.loadAndGetCubemap(skyboxFolderPath, skyboxFileType);
    _create_skybox_uniform_buffer(swapChain.currentSwapChainNbImg);
    _create_descriptor_layout();
    _create_pipeline_layout();
    _create_gfx_pipeline(swapChain);
    _pipeline_data = _create_pipeline_skybox();
    _create_descriptor_pool(swapChain, _pipeline_data);
    _create_descriptor_sets(swapChain, _pipeline_data, systemUbo);
}

void
VulkanSkyboxPipeline::resize(VulkanSwapChain const &swapChain,
                             VulkanTextureManager &texManager,
                             VkBuffer systemUbo)
{
    vkDestroyBuffer(_devices.device, _skybox_uniform, nullptr);
    vkFreeMemory(_devices.device, _skybox_uniform_memory, nullptr);
    vkDestroyPipeline(_devices.device, _graphic_pipeline, nullptr);
    vkDestroyPipelineLayout(_devices.device, _pipeline_layout, nullptr);
    _pipeline_render_pass.resize(swapChain);

    _skybox_tex =
      texManager.loadAndGetCubemap(_skybox_folder_path, _skybox_filetype);
    _create_skybox_uniform_buffer(swapChain.currentSwapChainNbImg);
    _create_pipeline_layout();
    _create_gfx_pipeline(swapChain);
    vkDestroyDescriptorPool(
      _devices.device, _pipeline_data.descriptorPool, nullptr);
    _create_descriptor_pool(swapChain, _pipeline_data);
    _create_descriptor_sets(swapChain, _pipeline_data, systemUbo);
}

void
VulkanSkyboxPipeline::clear()
{
    vkDestroyBuffer(_devices.device, _skybox_uniform, nullptr);
    vkFreeMemory(_devices.device, _skybox_uniform_memory, nullptr);
    vkDestroyPipeline(_devices.device, _graphic_pipeline, nullptr);
    vkDestroyPipelineLayout(_devices.device, _pipeline_layout, nullptr);
    _pipeline_render_pass.clear();
    vkDestroyDescriptorSetLayout(
      _devices.device, _descriptor_set_layout, nullptr);
    vkDestroyBuffer(_devices.device, _pipeline_data.buffer, nullptr);
    vkFreeMemory(_devices.device, _pipeline_data.memory, nullptr);
    vkDestroyDescriptorPool(
      _devices.device, _pipeline_data.descriptorPool, nullptr);
    _devices = VulkanDevices{};
    _cmdPools = VulkanCommandPools{};
    _queues = VulkanQueues{};
    _descriptor_set_layout = nullptr;
    _pipeline_layout = nullptr;
    _graphic_pipeline = nullptr;
    _pipeline_data.clear();
}

void
VulkanSkyboxPipeline::setSkyboxInfo(glm::mat4 const &skyboxInfo)
{
    _skybox_model = skyboxInfo;
}

VulkanSkyboxRenderPass const &
VulkanSkyboxPipeline::getVulkanSkyboxRenderPass() const
{
    return (_pipeline_render_pass);
}

void
VulkanSkyboxPipeline::generateCommands(VkCommandBuffer cmdBuffer,
                                       size_t descriptorSetIndex)
{
    // Vertex related values
    VkBuffer vertex_buffer[] = { _pipeline_data.buffer };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindPipeline(
      cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphic_pipeline);
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertex_buffer, offsets);
    vkCmdBindIndexBuffer(cmdBuffer,
                         _pipeline_data.buffer,
                         _pipeline_data.indicesOffset,
                         VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(cmdBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            _pipeline_layout,
                            0,
                            1,
                            &_pipeline_data.descriptorSets[descriptorSetIndex],
                            0,
                            nullptr);
    vkCmdDrawIndexed(cmdBuffer, _pipeline_data.indicesDrawNb, 1, 0, 0, 0);
}

void
VulkanSkyboxPipeline::setSkyboxModelMatOnGpu(uint32_t currentImg)
{
    copyOnCpuCoherentMemory(_devices.device,
                            _skybox_uniform_memory,
                            currentImg * sizeof(SkyboxUbo) +
                              offsetof(SkyboxUbo, model),
                            sizeof(glm::mat4),
                            &_skybox_model);
}

void
VulkanSkyboxPipeline::_create_descriptor_layout()
{
    VkDescriptorSetLayoutBinding system_ubo_layout_binding{};
    system_ubo_layout_binding.binding = 0;
    system_ubo_layout_binding.descriptorType =
      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    system_ubo_layout_binding.descriptorCount = 1;
    system_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    system_ubo_layout_binding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding skybox_ubo_layout_binding{};
    skybox_ubo_layout_binding.binding = 1;
    skybox_ubo_layout_binding.descriptorType =
      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    skybox_ubo_layout_binding.descriptorCount = 1;
    skybox_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    skybox_ubo_layout_binding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding sampler_layout_binding{};
    sampler_layout_binding.binding = 2;
    sampler_layout_binding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    sampler_layout_binding.descriptorCount = 1;
    sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    sampler_layout_binding.pImmutableSamplers = nullptr;

    std::array bindings{ system_ubo_layout_binding,
                         skybox_ubo_layout_binding,
                         sampler_layout_binding };

    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = bindings.size();
    layout_info.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(
          _devices.device, &layout_info, nullptr, &_descriptor_set_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanSkyboxPipeline: failed to create descriptor set layout");
    }
}

void
VulkanSkyboxPipeline::_create_pipeline_layout()
{
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &_descriptor_set_layout;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(
          _devices.device, &pipeline_layout_info, nullptr, &_pipeline_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanSkyboxPipeline: Failed to create pipeline layout");
    }
}

void
VulkanSkyboxPipeline::_create_gfx_pipeline(VulkanSwapChain const &swapChain)
{
    // Shaders
    auto vert_shader =
      loadShader(_devices.device, "resources/shaders/skybox/skybox.vert.spv");
    auto frag_shader =
      loadShader(_devices.device, "resources/shaders/skybox/skybox.frag.spv");

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
      VulkanSkyboxPipelineData::getInputBindingDescription();
    auto attribute_description =
      VulkanSkyboxPipelineData::getInputAttributeDescription();
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
    rasterizer_info.polygonMode = VK_POLYGON_MODE_FILL;
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
    if (vkCreateGraphicsPipelines(_devices.device,
                                  VK_NULL_HANDLE,
                                  1,
                                  &gfx_pipeline_info,
                                  nullptr,
                                  &_graphic_pipeline) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanSkyboxPipeline: Failed to create graphic pipeline");
    }

    vkDestroyShaderModule(_devices.device, vert_shader, nullptr);
    vkDestroyShaderModule(_devices.device, frag_shader, nullptr);
}

VulkanSkyboxPipelineData
VulkanSkyboxPipeline::_create_pipeline_skybox()
{
    VulkanSkyboxPipelineData pipeline_model{};
    SkyboxModel model;

    // Texture related
    pipeline_model.cubemapTexture = _skybox_tex;

    // Computing sizes and offsets
    pipeline_model.indicesDrawNb = model.getIndicesList().size();
    pipeline_model.verticesSize =
      sizeof(glm::vec3) * model.getVertexList().size();
    pipeline_model.indicesSize =
      sizeof(uint32_t) * model.getIndicesList().size();
    pipeline_model.indicesOffset = pipeline_model.verticesSize;
    VkDeviceSize total_size =
      pipeline_model.verticesSize + pipeline_model.indicesSize;

    // Creating transfer buffer CPU to GPU
    VkBuffer staging_buffer{};
    VkDeviceMemory staging_buffer_memory{};
    createBuffer(_devices.device,
                 staging_buffer,
                 total_size,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    allocateBuffer(_devices.physicalDevice,
                   _devices.device,
                   staging_buffer,
                   staging_buffer_memory,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // Copying data into staging buffer
    copyOnCpuCoherentMemory(_devices.device,
                            staging_buffer_memory,
                            0,
                            pipeline_model.verticesSize,
                            model.getVertexList().data());
    copyOnCpuCoherentMemory(_devices.device,
                            staging_buffer_memory,
                            pipeline_model.indicesOffset,
                            pipeline_model.indicesSize,
                            model.getIndicesList().data());

    // Creating GPU buffer + copying transfer buffer
    createBuffer(_devices.device,
                 pipeline_model.buffer,
                 total_size,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                   VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    allocateBuffer(_devices.physicalDevice,
                   _devices.device,
                   pipeline_model.buffer,
                   pipeline_model.memory,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    copyBufferOnGpu(_devices.device,
                    _cmdPools.renderCommandPool,
                    _queues.graphicQueue,
                    pipeline_model.buffer,
                    staging_buffer,
                    total_size);

    vkDestroyBuffer(_devices.device, staging_buffer, nullptr);
    vkFreeMemory(_devices.device, staging_buffer_memory, nullptr);

    return (pipeline_model);
}

void
VulkanSkyboxPipeline::_create_descriptor_pool(
  VulkanSwapChain const &swapChain,
  VulkanSkyboxPipelineData &pipelineData)
{
    std::array<VkDescriptorPoolSize, 3> pool_size{};
    // System Ubo
    pool_size[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_size[0].descriptorCount = swapChain.currentSwapChainNbImg;
    // Skybox Ubo
    pool_size[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_size[1].descriptorCount = swapChain.currentSwapChainNbImg;
    // Texture Sampler
    pool_size[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_size[2].descriptorCount = swapChain.currentSwapChainNbImg;

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = pool_size.size();
    pool_info.pPoolSizes = pool_size.data();
    pool_info.maxSets = swapChain.currentSwapChainNbImg;

    if (vkCreateDescriptorPool(
          _devices.device, &pool_info, nullptr, &pipelineData.descriptorPool) !=
        VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanSkyboxPipeline: failed to create descriptor pool");
    }
}

void
VulkanSkyboxPipeline::_create_descriptor_sets(
  VulkanSwapChain const &swapChain,
  VulkanSkyboxPipelineData &pipelineData,
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
          _devices.device, &alloc_info, pipelineData.descriptorSets.data()) !=
        VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanSkyboxPipeline: failed to create descriptor sets");
    }

    for (size_t i = 0; i < swapChain.currentSwapChainNbImg; ++i) {
        std::array<VkWriteDescriptorSet, 3> descriptor_write{};

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

        // Skybox UBO
        VkDescriptorBufferInfo model_buffer_info{};
        model_buffer_info.buffer = _skybox_uniform;
        model_buffer_info.offset = 0;
        model_buffer_info.range = sizeof(SkyboxUbo);
        descriptor_write[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write[1].dstSet = pipelineData.descriptorSets[i];
        descriptor_write[1].dstBinding = 1;
        descriptor_write[1].dstArrayElement = 0;
        descriptor_write[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_write[1].descriptorCount = 1;
        descriptor_write[1].pBufferInfo = &model_buffer_info;
        descriptor_write[1].pImageInfo = nullptr;
        descriptor_write[1].pTexelBufferView = nullptr;

        // Texture
        VkDescriptorImageInfo img_info{};
        img_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        img_info.imageView = pipelineData.cubemapTexture.texture_img_view;
        img_info.sampler = pipelineData.cubemapTexture.texture_sampler;
        descriptor_write[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write[2].dstSet = pipelineData.descriptorSets[i];
        descriptor_write[2].dstBinding = 2;
        descriptor_write[2].dstArrayElement = 0;
        descriptor_write[2].descriptorType =
          VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_write[2].descriptorCount = 1;
        descriptor_write[2].pBufferInfo = nullptr;
        descriptor_write[2].pImageInfo = &img_info;
        descriptor_write[2].pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(_devices.device,
                               descriptor_write.size(),
                               descriptor_write.data(),
                               0,
                               nullptr);
    }
}

void
VulkanSkyboxPipeline::_create_skybox_uniform_buffer(
  uint32_t currentSwapChainNbImg)
{
    createBuffer(_devices.device,
                 _skybox_uniform,
                 sizeof(SkyboxUbo) * currentSwapChainNbImg,
                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    allocateBuffer(_devices.physicalDevice,
                   _devices.device,
                   _skybox_uniform,
                   _skybox_uniform_memory,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}
