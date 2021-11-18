#include "particle/VulkanParticlePipelineData.hpp"

void
VulkanParticlePipelineData::init(VulkanDevices const &devices,
                                 uint32_t nbOfParticles)
{
    nbParticles = nbOfParticles;
    data.allocate(devices,
                  nbParticles * sizeof(VulkanParticle),
                  VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

void
VulkanParticlePipelineData::clear()
{
    data.clear();
    nbParticles = 0;
}