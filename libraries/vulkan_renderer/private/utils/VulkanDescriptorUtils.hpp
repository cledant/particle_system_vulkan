#ifndef PARTICLE_SYSTEM_VULKAN_VULKANDESCRIPTORUTILS_HPP
#define PARTICLE_SYSTEM_VULKAN_VULKANDESCRIPTORUTILS_HPP

#include <array>
#include <vector>
#include <stdexcept>

#include <vulkan/vulkan.h>

#include "common/VulkanCommonStruct.hpp"

template<size_t SIZE>
VkDescriptorSetLayout
createDescriptorSetLayout(
  VulkanDevices const &devices,
  std::array<VkDescriptorSetLayoutBinding, SIZE> const &bindings)
{
    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = bindings.size();
    layout_info.pBindings = bindings.data();

    VkDescriptorSetLayout dsl{};
    if (vkCreateDescriptorSetLayout(
          devices.device, &layout_info, nullptr, &dsl) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanDescriptorUtils: failed to create descriptor set layout");
    }
    return (dsl);
}

template<size_t SIZE>
VkDescriptorPool
createDescriptorPool(VulkanDevices const &devices,
                     std::array<VkDescriptorPoolSize, SIZE> poolSize,
                     uint32_t maxSets)
{
    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = poolSize.size();
    pool_info.pPoolSizes = poolSize.data();
    pool_info.maxSets = maxSets;

    VkDescriptorPool descriptorPool;
    if (vkCreateDescriptorPool(
          devices.device, &pool_info, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanDescriptorUtils: failed to create descriptor pool");
    }
    return (descriptorPool);
}

void allocateDescriptorSets(VulkanDevices const &devices,
                            VkDescriptorPool descriptorPool,
                            VkDescriptorSetLayout descriptorSetLayout,
                            uint32_t descriptorCount,
                            std::vector<VkDescriptorSet> &descriptorSets);

#endif // PARTICLE_SYSTEM_VULKAN_VULKANDESCRIPTORUTILS_HPP
