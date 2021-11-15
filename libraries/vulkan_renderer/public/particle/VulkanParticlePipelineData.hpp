#ifndef PARTICLE_SYS_VULKANPARTICLEPIPELINEDATA_HPP
#define PARTICLE_SYS_VULKANPARTICLEPIPELINEDATA_HPP

#include <array>
#include <vector>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

#include "tex/VulkanTextureManager.hpp"

struct VulkanParticle
{
    alignas(16) glm::vec3 position;
};

struct VulkanParticlePipelineData
{
    VkBuffer buffer{};
    VkDeviceMemory memory{};
    VkDeviceSize nbParticles;
    VkDeviceSize particleBufferSize{};
    VkDescriptorPool descriptorPool{};
    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorSet computeDescriptorSet;

    static std::array<VkVertexInputBindingDescription, 1>
    getInputBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 1>
    getInputAttributeDescription();

    void clear();
};

#endif // PARTICLE_SYS_VULKANPARTICLEPIPELINEDATA_HPP
