#ifndef PARTICLE_SYSTEM_VULKAN_VULKANPARTICLEGFXPIPELINEDESCRIPTION_HPP
#define PARTICLE_SYSTEM_VULKAN_VULKANPARTICLEGFXPIPELINEDESCRIPTION_HPP

#include <array>

#include "glm/glm.hpp"

#include "interface/AVulkanPipelineDescription.hpp"
#include "VulkanParticlePipelineData.hpp"

class VulkanParticleGfxPipelineDescription
  : public AVulkanPipelineDescription<VulkanParticleGfxPipelineDescription>
{
  public:
    VulkanParticleGfxPipelineDescription() = default;
    ~VulkanParticleGfxPipelineDescription() override = default;
    VulkanParticleGfxPipelineDescription(
      VulkanParticleGfxPipelineDescription const &src) = default;
    VulkanParticleGfxPipelineDescription &operator=(
      VulkanParticleGfxPipelineDescription const &rhs) = default;
    VulkanParticleGfxPipelineDescription(
      VulkanParticleGfxPipelineDescription &&src) noexcept = default;
    VulkanParticleGfxPipelineDescription &operator=(
      VulkanParticleGfxPipelineDescription &&rhs) noexcept = default;

    void implInit(VulkanDevices const &devices);
    void implClear();

    static constexpr std::array<VkVertexInputBindingDescription, 1> const
      inputBindingDescription{
          { { 0, sizeof(VulkanParticle), VK_VERTEX_INPUT_RATE_VERTEX } }
      };
    static constexpr std::array<VkVertexInputAttributeDescription, 1> const
      inputAttributeDescription{ { { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 } } };

  protected:
    static constexpr std::array<VkDescriptorSetLayoutBinding, 3> const
      descriptorBindings{ { { 0,
                              VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                              1,
                              VK_SHADER_STAGE_VERTEX_BIT,
                              nullptr },
                            { 1,
                              VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                              1,
                              VK_SHADER_STAGE_VERTEX_BIT,
                              nullptr } } };
};

#endif // PARTICLE_SYSTEM_VULKAN_VULKANPARTICLEGFXPIPELINEDESCRIPTION_HPP
