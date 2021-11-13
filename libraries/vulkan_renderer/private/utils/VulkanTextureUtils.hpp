#ifndef PARTICLE_SYSTEM_VULKAN_VULKANTEXTUREUTILS_HPP
#define PARTICLE_SYSTEM_VULKAN_VULKANTEXTUREUTILS_HPP

#include <cmath>

#include <vulkan/vulkan.h>

#include "common/VulkanCommonStruct.hpp"

uint32_t constexpr computeMipmapLevel(int32_t texW, int32_t texH)
{
    return (static_cast<uint32_t>(std::floor(std::log2(std::max(texW, texH)))) +
            1);
}
void generateMipmaps(VulkanDevices const &devices,
                     VulkanCommandPools const &cmdPools,
                     VulkanQueues const &queues,
                     VulkanTexture const &texture);
void createTextureSampler(VulkanDevices const &devices, VulkanTexture &texture);

#endif // PARTICLE_SYSTEM_VULKAN_VULKANTEXTUREUTILS_HPP
