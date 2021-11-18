#include "particle/VulkanParticleGfxPipelineDescription.hpp"

#include "utils/VulkanDescriptorUtils.hpp"
#include "utils/VulkanPipelineUtils.hpp"

void
VulkanParticleGfxPipelineDescription::implInit(VulkanDevices const &devices)
{
    descriptorSetLayout =
      createDescriptorSetLayout(devices, descriptorBindings);
    pipelineLayout = createPipelineLayout(devices, descriptorSetLayout);
}

void
VulkanParticleGfxPipelineDescription::implClear()
{}
