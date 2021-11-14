#include "VulkanPipelineUtils.hpp"

#include <stdexcept>

VkPipelineLayout
createPipelineLayout(VulkanDevices const &devices,
                     VkDescriptorSetLayout descriptorSetLayout)
{
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &descriptorSetLayout;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;

    VkPipelineLayout pl{};
    if (vkCreatePipelineLayout(
          devices.device, &pipeline_layout_info, nullptr, &pl) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanPipelineUtils: Failed to create pipeline layout");
    }
    return (pl);
}