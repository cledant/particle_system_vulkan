#include "particle/VulkanParticleComputePipelineDescription.hpp"

#include "utils/VulkanDescriptorUtils.hpp"
#include "utils/VulkanPipelineUtils.hpp"
#include "utils/VulkanPhysicalDevice.hpp"

void
VulkanParticleComputePipelineDescription::implInit(VulkanDevices const &devices)
{
    descriptorSetLayout =
      createDescriptorSetLayout(devices, descriptorBindings);
    pipelineLayout = createPipelineLayout(devices, descriptorSetLayout);
    workGroupSize =
      std::min(DEFAULT_COMPUTE_WORK_GROUP_SIZE,
               getPhysicalDeviceProperties(_devices.physicalDevice)
                 .limits.maxComputeWorkGroupSize[0]);
}

void
VulkanParticleComputePipelineDescription::implClear()
{
    workGroupSize = 0;
}
