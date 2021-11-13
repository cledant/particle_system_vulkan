#include "common/VulkanCommonStruct.hpp"

#include "utils/VulkanImage.hpp"
#include "utils/VulkanTextureUtils.hpp"

void
VulkanTexture::loadTextureOnGPU(VulkanDevices const &devices,
                                VulkanCommandPools const &cmdPools,
                                VulkanQueues const &queues,
                                VulkanTextureStaging const &stagingTexture,
                                VkFormat format)
{
    _devices = devices;
    textureFormat = format;
    width = stagingTexture.width;
    height = stagingTexture.height;
    mipLevel = stagingTexture.mipLevel;
    isCubemap = stagingTexture.isCubemap;

    createImage(_devices.device,
                *this,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                  VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    allocateImage(_devices, *this, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    transitionImageLayout(_devices,
                          cmdPools,
                          queues,
                          *this,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(
      _devices, cmdPools, queues, stagingTexture.stagingBuffer, *this);
    generateMipmaps(_devices, cmdPools, queues, *this);
    createImageView(_devices, *this, VK_IMAGE_ASPECT_COLOR_BIT);
    createTextureSampler(devices, *this);
}

void
VulkanTexture::createDepthTexture(VulkanDevices const &devices,
                                  VulkanCommandPools const &cmdPools,
                                  VulkanQueues const &queues,
                                  int32_t texW,
                                  int32_t texH,
                                  VkFormat depthFormat)
{
    _devices = devices;
    textureFormat = depthFormat;
    width = texW;
    height = texH;

    createImage(_devices.device,
                *this,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    allocateImage(_devices, *this, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    createImageView(devices, *this, VK_IMAGE_ASPECT_DEPTH_BIT);
    transitionImageLayout(devices,
                          cmdPools,
                          queues,
                          *this,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void
VulkanTexture::createSwapchainTexture(VulkanDevices const &devices,
                                      VkImage swapchainImg,
                                      VkFormat swapchainFormat,
                                      VkExtent2D extent)
{
    _devices = devices;
    width = extent.width;
    height = extent.height;
    textureFormat = swapchainFormat;
    textureImg = swapchainImg;
    createImageView(_devices, *this, VK_IMAGE_ASPECT_COLOR_BIT);
}

void
VulkanTexture::clear()
{
    if (textureSampler) {
        vkDestroySampler(_devices.device, textureSampler, nullptr);
    }
    if (textureImgView) {
        vkDestroyImageView(_devices.device, textureImgView, nullptr);
    }
    if (textureImg) {
        vkDestroyImage(_devices.device, textureImg, nullptr);
    }
    if (textureImgMemory) {
        vkFreeMemory(_devices.device, textureImgMemory, nullptr);
    }
    textureSampler = nullptr;
    textureImgView = nullptr;
    textureImg = nullptr;
    textureImgMemory = nullptr;
    isCubemap = false;
    textureFormat = static_cast<VkFormat>(0);
    width = 0;
    height = 0;
    _devices = VulkanDevices{};
}

void
VulkanTexture::clearSwapchainTexture()
{
    if (textureImgView) {
        vkDestroyImageView(_devices.device, textureImgView, nullptr);
    }
    textureSampler = nullptr;
    textureImgView = nullptr;
    textureImg = nullptr;
    textureImgMemory = nullptr;
    isCubemap = false;
    textureFormat = static_cast<VkFormat>(0);
    width = 0;
    height = 0;
    _devices = VulkanDevices{};
}
