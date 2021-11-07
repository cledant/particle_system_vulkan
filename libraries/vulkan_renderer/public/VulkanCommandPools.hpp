
#ifndef PARTICLE_SYSTEM_VULKAN_VULKANCOMMANDPOOLS_HPP
#define PARTICLE_SYSTEM_VULKAN_VULKANCOMMANDPOOLS_HPP

struct VulkanCommandPools
{
    VkCommandPool renderCommandPool{};
    VkCommandPool computeCommandPool{};
};

#endif // PARTICLE_SYSTEM_VULKAN_VULKANCOMMANDPOOLS_HPP
