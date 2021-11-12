#ifndef PARTICLE_SYS_VULKAN_VULKANIMAGE_HPP
#define PARTICLE_SYS_VULKAN_VULKANIMAGE_HPP

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#include "common/VulkanCommonStruct.hpp"

void createImage(VkDevice device,
                 VulkanTexture &texture,
                 VkImageTiling tiling,
                 VkImageUsageFlags usage);
void allocateImage(VulkanDevices devices,
                   VulkanTexture &texture,
                   VkMemoryPropertyFlags properties);
void transitionImageLayout(VulkanDevices devices,
                           VulkanCommandPools cmdPools,
                           VulkanQueues queues,
                           VulkanTexture &texture,
                           VkImageLayout old_layout,
                           VkImageLayout new_layout);
void copyBufferToImage(VulkanDevices devices,
                       VulkanCommandPools cmdPools,
                       VulkanQueues queues,
                       VulkanBuffer stagingBuffer,
                       VulkanTexture texture);
void createImageView(VulkanDevices devices,
                     VulkanTexture &texture,
                     VkImageAspectFlags aspect_flags);
VkFormat findSupportedFormat(VkPhysicalDevice physical_device,
                             std::vector<VkFormat> const &candidates,
                             VkImageTiling tiling,
                             VkFormatFeatureFlags features);
bool hasStencilComponent(VkFormat format);

#endif // PARTICLE_SYS_VULKAN_VULKANIMAGE_HPP
