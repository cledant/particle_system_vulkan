#include "VulkanDescriptorUtils.hpp"

void
allocateDescriptorSets(VulkanDevices const &devices,
                       VkDescriptorPool descriptorPool,
                       VkDescriptorSetLayout descriptorSetLayout,
                       uint32_t descriptorCount,
                       std::vector<VkDescriptorSet> &descriptorSets)
{
    std::vector<VkDescriptorSetLayout> layouts(descriptorCount,
                                               descriptorSetLayout);

    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptorPool;
    alloc_info.descriptorSetCount = descriptorCount;
    alloc_info.pSetLayouts = layouts.data();

    descriptorSets.resize(descriptorCount);
    if (vkAllocateDescriptorSets(
          devices.device, &alloc_info, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanDescriptorUtils: failed to create descriptor sets");
    }
}
