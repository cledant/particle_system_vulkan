#include "skybox/VulkanSkyboxPipelineData.hpp"

#include "skybox/SkyboxModel.hpp"
#include "utils/VulkanMemory.hpp"

void
VulkanSkyboxPipelineData::init(VulkanDevices const &devices,
                               VulkanCommandPools const &cmdPools,
                               VulkanQueues const &queues,
                               VulkanTexture const &skyboxTex)
{
    SkyboxModel model;

    // Texture related
    cubemapTexture = skyboxTex;

    // Computing sizes and offsets
    indicesDrawNb = model.getIndicesList().size();
    verticesSize = sizeof(glm::vec3) * model.getVertexList().size();
    indicesSize = sizeof(uint32_t) * model.getIndicesList().size();
    indicesOffset = verticesSize;
    VkDeviceSize total_size = verticesSize + indicesSize;

    // Creating transfer buffer CPU to GPU
    VulkanBuffer staging_buff{};
    staging_buff.allocate(devices,
                          total_size,
                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // Copying data into staging buffer
    copyOnCpuCoherentMemory(devices.device,
                            staging_buff.memory,
                            0,
                            verticesSize,
                            model.getVertexList().data());
    copyOnCpuCoherentMemory(devices.device,
                            staging_buff.memory,
                            indicesOffset,
                            indicesSize,
                            model.getIndicesList().data());

    // Creating GPU buffer + copying transfer buffer
    data.allocate(devices,
                  total_size,
                  VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    copyBufferOnGpu(devices.device,
                    cmdPools.renderCommandPool,
                    queues.graphicQueue,
                    data.buffer,
                    staging_buff.buffer,
                    total_size);

    staging_buff.clear();
}

void
VulkanSkyboxPipelineData::clear()
{
    data.clear();
    verticesSize = 0;
    indicesSize = 0;
    indicesOffset = 0;
    verticesSize = 0;
    cubemapTexture = VulkanTexture{};
    indicesDrawNb = 0;
}