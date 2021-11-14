#ifndef PARTICLE_SYSTEM_VULKAN_VULKANPIPELINEUTILS_HPP
#define PARTICLE_SYSTEM_VULKAN_VULKANPIPELINEUTILS_HPP

#include "common/VulkanCommonStruct.hpp"

VkPipelineLayout createPipelineLayout(
  VulkanDevices const &devices,
  VkDescriptorSetLayout descriptorSetLayout);

#endif // PARTICLE_SYSTEM_VULKAN_VULKANPIPELINEUTILS_HPP
