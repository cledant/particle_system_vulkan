#ifndef PARTICLE_SYS_VULKANSKYBOXPIPELINEDATA_HPP
#define PARTICLE_SYS_VULKANSKYBOXPIPELINEDATA_HPP

#include <array>
#include <vector>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

#include "tex/VulkanTextureManager.hpp"
#include "ubo/VulkanUboStructs.hpp"

struct VulkanSkyboxPipelineData
{
    VulkanBuffer data{};
    VkDeviceSize verticesSize{};
    VkDeviceSize indicesSize{};
    VkDeviceSize indicesOffset{};
    VulkanTexture cubemapTexture;
    VkDeviceSize indicesDrawNb{};

    void init(VulkanDevices const &devices,
              VulkanCommandPools const &cmdPools,
              VulkanQueues const &queues,
              VulkanTexture const &skyboxTex);
    void clear();
};

#endif // PARTICLE_SYS_VULKANSKYBOXPIPELINEDATA_HPP
