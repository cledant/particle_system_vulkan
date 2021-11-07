#ifndef PARTICLE_SYSTEM_VULKAN_VULKANQUEUES_HPP
#define PARTICLE_SYSTEM_VULKAN_VULKANQUEUES_HPP

#include <vulkan/vulkan.h>

struct VulkanQueues
{
    VkQueue graphicQueue{};
    VkQueue presentQueue{};
    VkQueue computeQueue{};
    uint32_t graphicQueueIndex{};
    uint32_t presentQueueIndex{};
    uint32_t computeQueueIndex{};
};

#endif // PARTICLE_SYSTEM_VULKAN_VULKANQUEUES_HPP
