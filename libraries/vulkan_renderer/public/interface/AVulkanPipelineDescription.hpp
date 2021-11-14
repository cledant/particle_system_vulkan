#ifndef PARTICLE_SYSTEM_VULKAN_AVULKANPIPELINEDESCRIPTION_HPP
#define PARTICLE_SYSTEM_VULKAN_AVULKANPIPELINEDESCRIPTION_HPP

#include "common/VulkanCommonStruct.hpp"

template<class Child>
class AVulkanPipelineDescription
{
  public:
    AVulkanPipelineDescription() = default;
    virtual ~AVulkanPipelineDescription() = default;
    AVulkanPipelineDescription(AVulkanPipelineDescription const &src) = default;
    AVulkanPipelineDescription &operator=(
      AVulkanPipelineDescription const &rhs) = default;
    AVulkanPipelineDescription(AVulkanPipelineDescription &&src) noexcept =
      default;
    AVulkanPipelineDescription &operator=(
      AVulkanPipelineDescription &&rhs) noexcept = default;

    void init(VulkanDevices const &devices, uint32_t descriptorCount);
    void resize(uint32_t descriptorCount);
    void clear();

    VkDescriptorSetLayout descriptorSetLayout{};
    VkPipelineLayout pipelineLayout{};
    VkDescriptorPool descriptorPool{};

  protected:
    VulkanDevices _devices;
};

template<class Child>
void
AVulkanPipelineDescription<Child>::init(VulkanDevices const &devices,
                                        uint32_t descriptorCount)
{
    _devices = devices;
    static_cast<Child &>(*this).implInit(devices, descriptorCount);
}

template<class Child>
void
AVulkanPipelineDescription<Child>::resize(uint32_t descriptorCount)
{
    vkDestroyDescriptorPool(_devices.device, descriptorPool, nullptr);
    descriptorPool = nullptr;
    static_cast<Child &>(*this).implResize(descriptorCount);
}

template<class Child>
void
AVulkanPipelineDescription<Child>::clear()
{
    static_cast<Child &>(*this).implClear();
    vkDestroyDescriptorPool(_devices.device, descriptorPool, nullptr);
    vkDestroyPipelineLayout(_devices.device, pipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(_devices.device, descriptorSetLayout, nullptr);
    descriptorSetLayout = nullptr;
    pipelineLayout = nullptr;
    descriptorPool = nullptr;
}

#endif // PARTICLE_SYSTEM_VULKAN_AVULKANPIPELINEDESCRIPTION_HPP
