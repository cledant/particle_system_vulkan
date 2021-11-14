#ifndef PARTICLE_SYSTEM_VULKAN_VULKANSKYBOXPIPELINEDESCRIPTIONPRIVATE_HPP
#define PARTICLE_SYSTEM_VULKAN_VULKANSKYBOXPIPELINEDESCRIPTIONPRIVATE_HPP

#include "skybox/VulkanSkyboxPipelineDescription.hpp"

#include "utils/VulkanDescriptorUtils.hpp"
#include "utils/VulkanPipelineUtils.hpp"

void
VulkanSkyboxPipelineDescription::implInit(VulkanDevices const &devices,
                                          uint32_t descriptorCount)
{
    descriptorSetLayout =
      createDescriptorSetLayout(devices, descriptorBindings);
    pipelineLayout = createPipelineLayout(devices, descriptorSetLayout);
    createDescriptorPool(descriptorCount);
}

void
VulkanSkyboxPipelineDescription::implResize(uint32_t descriptorCount)
{
    createDescriptorPool(descriptorCount);
}

void
VulkanSkyboxPipelineDescription::implClear()
{}

void
VulkanSkyboxPipelineDescription::createDescriptorPool(uint32_t descriptorCount)
{
    std::array<VkDescriptorPoolSize, 3> const poolSize{
        { { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptorCount },
          { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptorCount },
          { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptorCount } }
    };

    descriptorPool =
      ::createDescriptorPool(_devices, poolSize, descriptorCount);
}

#endif // PARTICLE_SYSTEM_VULKAN_VULKANSKYBOXPIPELINEDESCRIPTIONPRIVATE_HPP
