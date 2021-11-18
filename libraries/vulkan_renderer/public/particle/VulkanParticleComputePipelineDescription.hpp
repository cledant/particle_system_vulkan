#ifndef PARTICLE_SYSTEM_VULKAN_VULKANPARTICLECOMPUTEPIPELINEDESCRIPTION_HPP
#define PARTICLE_SYSTEM_VULKAN_VULKANPARTICLECOMPUTEPIPELINEDESCRIPTION_HPP

#include <array>

#include "glm/glm.hpp"

#include "interface/AVulkanPipelineDescription.hpp"

class VulkanParticleComputePipelineDescription
  : public AVulkanPipelineDescription<VulkanParticleComputePipelineDescription>
{
  public:
    VulkanParticleComputePipelineDescription() = default;
    ~VulkanParticleComputePipelineDescription() override = default;
    VulkanParticleComputePipelineDescription(
      VulkanParticleComputePipelineDescription const &src) = default;
    VulkanParticleComputePipelineDescription &operator=(
      VulkanParticleComputePipelineDescription const &rhs) = default;
    VulkanParticleComputePipelineDescription(
      VulkanParticleComputePipelineDescription &&src) noexcept = default;
    VulkanParticleComputePipelineDescription &operator=(
      VulkanParticleComputePipelineDescription &&rhs) noexcept = default;

    static constexpr uint32_t const DEFAULT_COMPUTE_WORK_GROUP_SIZE = 256;

    void implInit(VulkanDevices const &devices);
    void implClear();

    uint32_t workGroupSize{};

  protected:
    static constexpr std::array<VkDescriptorSetLayoutBinding, 3> const
      descriptorBindings{ { { 0,
                              VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                              1,
                              VK_SHADER_STAGE_COMPUTE_BIT,
                              nullptr },
                            { 1,
                              VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                              1,
                              VK_SHADER_STAGE_COMPUTE_BIT,
                              nullptr } } };
};

#endif // PARTICLE_SYSTEM_VULKAN_VULKANPARTICLECOMPUTEPIPELINEDESCRIPTION_HPP
