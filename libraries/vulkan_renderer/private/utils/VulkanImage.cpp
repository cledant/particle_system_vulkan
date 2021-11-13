#include "VulkanImage.hpp"

#include <stdexcept>

#include "VulkanMemory.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanPhysicalDevice.hpp"

void
createImage(VkDevice device,
            VulkanTexture &texture,
            VkImageTiling tiling,
            VkImageUsageFlags usage)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = texture.width;
    imageInfo.extent.height = texture.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = texture.mipLevel;
    imageInfo.arrayLayers = (texture.isCubemap) ? 6 : 1;
    imageInfo.format = texture.textureFormat;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (texture.isCubemap) {
        imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    }

    if (vkCreateImage(device, &imageInfo, nullptr, &texture.textureImg) !=
        VK_SUCCESS) {
        throw std::runtime_error("VkImage: failed to create image");
    }
}

void
allocateImage(VulkanDevices const &devices,
              VulkanTexture &texture,
              VkMemoryPropertyFlags properties)
{
    VkMemoryRequirements mem_req{};
    vkGetImageMemoryRequirements(devices.device, texture.textureImg, &mem_req);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_req.size;
    alloc_info.memoryTypeIndex = findMemoryType(
      devices.physicalDevice, mem_req.memoryTypeBits, properties);

    if (vkAllocateMemory(
          devices.device, &alloc_info, nullptr, &texture.textureImgMemory) !=
        VK_SUCCESS) {
        throw std::runtime_error("VkImage: failed to allocate image memory");
    }
    vkBindImageMemory(
      devices.device, texture.textureImg, texture.textureImgMemory, 0);
}

void
transitionImageLayout(VulkanDevices const &devices,
                      VulkanCommandPools const &cmdPools,
                      VulkanQueues const &queues,
                      VulkanTexture const &texture,
                      VkImageLayout old_layout,
                      VkImageLayout new_layout)
{
    auto cmd_buffer =
      beginSingleTimeCommands(devices.device, cmdPools.renderCommandPool);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = texture.textureImg;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = texture.mipLevel;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = (texture.isCubemap) ? 6 : 1;
    if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (hasStencilComponent(texture.textureFormat)) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    } else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    VkPipelineStageFlags source_stage;
    VkPipelineStageFlags destination_stage;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
        new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
               new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else {
        throw std::runtime_error("VkImage: unsupported layout transition");
    }

    vkCmdPipelineBarrier(cmd_buffer,
                         source_stage,
                         destination_stage,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &barrier);
    endSingleTimeCommands(devices.device,
                          cmdPools.renderCommandPool,
                          cmd_buffer,
                          queues.graphicQueue);
}

void
copyBufferToImage(VulkanDevices const &devices,
                  VulkanCommandPools const &cmdPools,
                  VulkanQueues const &queues,
                  VulkanBuffer const &stagingBuffer,
                  VulkanTexture const &texture)
{
    auto cmd_buffer =
      beginSingleTimeCommands(devices.device, cmdPools.renderCommandPool);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = (texture.isCubemap) ? 6 : 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { static_cast<uint32_t>(texture.width),
                           static_cast<uint32_t>(texture.height),
                           1 };
    vkCmdCopyBufferToImage(cmd_buffer,
                           stagingBuffer.buffer,
                           texture.textureImg,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &region);
    endSingleTimeCommands(devices.device,
                          cmdPools.renderCommandPool,
                          cmd_buffer,
                          queues.graphicQueue);
}

void
createImageView(VulkanDevices const &devices,
                VulkanTexture &texture,
                VkImageAspectFlags aspect_flags)
{
    VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = texture.textureImg;
    create_info.viewType =
      (texture.isCubemap) ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = texture.textureFormat;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = aspect_flags;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = texture.mipLevel;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = (texture.isCubemap) ? 6 : 1;

    if (vkCreateImageView(
          devices.device, &create_info, nullptr, &texture.textureImgView) !=
        VK_SUCCESS) {
        throw std::runtime_error("VkRenderer: failed to create image view");
    }
}

VkFormat
findSupportedFormat(VkPhysicalDevice physical_device,
                    std::vector<VkFormat> const &candidates,
                    VkImageTiling tiling,
                    VkFormatFeatureFlags features)
{
    for (auto const &it : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physical_device, it, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR &&
            (props.linearTilingFeatures & features) == features) {
            return (it);
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
                   (props.optimalTilingFeatures & features) == features) {
            return (it);
        }
    }
    throw std::runtime_error("VkImage: failed to find supported format");
}

bool
hasStencilComponent(VkFormat format)
{
    return (format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
            format == VK_FORMAT_D24_UNORM_S8_UINT);
}
