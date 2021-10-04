#ifndef PARTICLE_SYS_VULKANSKYBOXPIPELINEDATA_HPP
#define PARTICLE_SYS_VULKANSKYBOXPIPELINEDATA_HPP

#include <array>
#include <vector>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

#include "tex/VulkanTextureManager.hpp"

struct VulkanSkyboxPipelineData
{
    VkBuffer buffer{};
    VkDeviceMemory memory{};
    VkDeviceSize verticesSize{};
    VkDeviceSize indicesSize{};
    VkDeviceSize singleUboSize{};
    VkDeviceSize singleSwapChainUboSize{};
    VkDeviceSize instanceMatricesOffset{};
    VkDeviceSize indicesOffset{};
    VkDeviceSize uboOffset{};
    VkDescriptorPool descriptorPool{};
    std::vector<VkDescriptorSet> descriptorSets;
    glm::vec3 modelCenter{};
    VkDeviceSize nbMaterials{};
    std::vector<Texture> diffuseTextures;
    std::vector<VkDeviceSize> indicesDrawOffset;
    std::vector<VkDeviceSize> indicesDrawNb;

    static std::array<VkVertexInputBindingDescription, 2>
    getInputBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 9>
    getInputAttributeDescription();

    void clear();
};

#endif // PARTICLE_SYS_VULKANSKYBOXPIPELINEDATA_HPP
