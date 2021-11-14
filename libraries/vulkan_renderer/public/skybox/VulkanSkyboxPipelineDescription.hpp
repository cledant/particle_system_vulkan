#ifndef PARTICLE_SYSTEM_VULKAN_VULKANSKYBOXPIPELINEDESCRIPTION_HPP
#define PARTICLE_SYSTEM_VULKAN_VULKANSKYBOXPIPELINEDESCRIPTION_HPP

#include <array>

#include "glm/glm.hpp"

#include "interface/AVulkanPipelineDescription.hpp"

class VulkanSkyboxPipelineDescription
  : public AVulkanPipelineDescription<VulkanSkyboxPipelineDescription>
{
  public:
    VulkanSkyboxPipelineDescription() = default;
    ~VulkanSkyboxPipelineDescription() override = default;
    VulkanSkyboxPipelineDescription(
      VulkanSkyboxPipelineDescription const &src) = default;
    VulkanSkyboxPipelineDescription &operator=(
      VulkanSkyboxPipelineDescription const &rhs) = default;
    VulkanSkyboxPipelineDescription(
      VulkanSkyboxPipelineDescription &&src) noexcept = default;
    VulkanSkyboxPipelineDescription &operator=(
      VulkanSkyboxPipelineDescription &&rhs) noexcept = default;

    void implInit(VulkanDevices const &devices, uint32_t maxPool);
    void implResize(uint32_t maxPool);
    void implClear();

    static constexpr std::array<VkVertexInputBindingDescription, 1> const
      inputBindingDescription{
          { { 0, sizeof(glm::vec3), VK_VERTEX_INPUT_RATE_VERTEX } }
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
                              nullptr },
                            { 2,
                              VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                              1,
                              VK_SHADER_STAGE_FRAGMENT_BIT,
                              nullptr } } };

    void createDescriptorPool(uint32_t descriptorCount);
};

#endif // PARTICLE_SYSTEM_VULKAN_VULKANSKYBOXPIPELINEDESCRIPTION_HPP
