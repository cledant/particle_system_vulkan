#ifndef PARTICLE_SYSTEM_VULKAN_VULKANTEXTURE_HPP
#define PARTICLE_SYSTEM_VULKAN_VULKANTEXTURE_HPP

#include <vulkan/vulkan.h>

struct VulkanTexture final
{
    VkImage textureImg{};
    VkDeviceMemory textureImgMemory{};
    VkImageView textureImgView{};
    VkSampler textureSampler{};
    int32_t width{};
    int32_t height{};
    uint32_t mipLevel{};
};

#endif // PARTICLE_SYSTEM_VULKAN_VULKANTEXTURE_HPP
