#include "VulkanTextureUtils.hpp"

#include <stdexcept>

#include "VulkanCommandBuffer.hpp"
#include "VulkanPhysicalDevice.hpp"

void
generateMipmaps(VulkanDevices devices,
                VulkanCommandPools cmdPools,
                VulkanQueues queues,
                VulkanTexture const &texture)
{
    if (!getLinearBlittingSupport(devices.physicalDevice,
                                  texture.textureFormat)) {
        throw std::runtime_error(
          "VulkanTextureUtils: Linear Blitting not supported "
          "for requested image format");
    }

    uint32_t nb_face = (texture.isCubemap) ? 6 : 1;
    for (uint32_t j = 0; j < nb_face; j++) {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = texture.textureImg;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = j;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mip_w = texture.width;
        int32_t mip_h = texture.height;
        VkCommandBuffer cmd_buffer =
          beginSingleTimeCommands(devices.device, cmdPools.renderCommandPool);
        for (uint32_t i = 1; i < texture.mipLevel; i++) {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            vkCmdPipelineBarrier(cmd_buffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 0,
                                 0,
                                 nullptr,
                                 0,
                                 nullptr,
                                 1,
                                 &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mip_w, mip_h, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = j;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { (mip_w > 1) ? mip_w / 2 : 1,
                                   (mip_h > 1) ? mip_h / 2 : 1,
                                   1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = j;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(cmd_buffer,
                           texture.textureImg,
                           VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           texture.textureImg,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &blit,
                           VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            vkCmdPipelineBarrier(cmd_buffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                 0,
                                 0,
                                 nullptr,
                                 0,
                                 nullptr,
                                 1,
                                 &barrier);
            if (mip_w > 1) {
                mip_w /= 2;
            }
            if (mip_h > 1) {
                mip_h /= 2;
            }
        }
        barrier.subresourceRange.baseMipLevel = texture.mipLevel - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier(cmd_buffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
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
}

void
createTextureSampler(VulkanDevices devices, VulkanTexture &texture)
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(devices.physicalDevice, &properties);
    float aniso = (properties.limits.maxSamplerAnisotropy > 16.0f)
                    ? 16.0f
                    : properties.limits.maxSamplerAnisotropy;

    VkSamplerCreateInfo sampler_info{};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = VK_FILTER_LINEAR;
    sampler_info.minFilter = VK_FILTER_LINEAR;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.anisotropyEnable = VK_TRUE;
    sampler_info.maxAnisotropy = aniso;
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = static_cast<float>(texture.mipLevel);

    if (vkCreateSampler(
          devices.device, &sampler_info, nullptr, &texture.textureSampler) !=
        VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanTextureUtils: failed to create texture sampler");
    }
}