#ifndef PARTICLE_SYSTEM_VULKAN_VULKANCOMMONSTRUCT_HPP
#define PARTICLE_SYSTEM_VULKAN_VULKANCOMMONSTRUCT_HPP

#include <vulkan/vulkan.h>

struct VulkanDevices
{
    VkPhysicalDevice physicalDevice{};
    VkDevice device{};
};

struct VulkanCommandPools
{
    VkCommandPool renderCommandPool{};
    VkCommandPool computeCommandPool{};
};

struct VulkanQueues
{
    VkQueue graphicQueue{};
    VkQueue presentQueue{};
    VkQueue computeQueue{};
    uint32_t graphicQueueIndex{};
    uint32_t presentQueueIndex{};
    uint32_t computeQueueIndex{};
};

struct VulkanBuffer
{
    VkBuffer buffer{};
    VkDeviceMemory memory{};

    void allocate(VulkanDevices devices,
                  VkDeviceSize size,
                  VkBufferUsageFlags usage,
                  VkMemoryPropertyFlags properties);
    void clear();

  private:
    VulkanDevices _devices{};
};

#endif // PARTICLE_SYSTEM_VULKAN_VULKANCOMMONSTRUCT_HPP
