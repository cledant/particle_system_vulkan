#include "skybox/VulkanSkyboxPipelineDescription.hpp"

#include "utils/VulkanDescriptorUtils.hpp"
#include "utils/VulkanPipelineUtils.hpp"

void
VulkanSkyboxPipelineDescription::implInit(VulkanDevices const &devices)
{
    descriptorSetLayout =
      createDescriptorSetLayout(devices, descriptorBindings);
    pipelineLayout = createPipelineLayout(devices, descriptorSetLayout);
}

void
VulkanSkyboxPipelineDescription::implClear()
{}
