#ifndef PARTICLE_SYS_VULKANSKYBOXPIPELINEDATA_HPP
#define PARTICLE_SYS_VULKANSKYBOXPIPELINEDATA_HPP

#include <array>
#include <vector>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

#include "tex/VulkanTextureManager.hpp"

struct VulkanSkyboxPipelineData
{
    VulkanBuffer data{};
    VkDeviceSize verticesSize{};
    VkDeviceSize indicesSize{};
    VkDeviceSize indicesOffset{};
    VkDescriptorPool descriptorPool{};
    std::vector<VkDescriptorSet> descriptorSets;
    VulkanTexture cubemapTexture;
    VkDeviceSize indicesDrawNb;

    static std::array<VkVertexInputBindingDescription, 1>
    getInputBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 1>
    getInputAttributeDescription();

    void clear();
};

#endif // PARTICLE_SYS_VULKANSKYBOXPIPELINEDATA_HPP
