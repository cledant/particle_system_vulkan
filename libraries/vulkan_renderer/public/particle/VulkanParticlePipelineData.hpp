#ifndef PARTICLE_SYS_VULKANPARTICLEPIPELINEDATA_HPP
#define PARTICLE_SYS_VULKANPARTICLEPIPELINEDATA_HPP

#include <array>
#include <vector>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

#include "common/VulkanCommonStruct.hpp"

struct VulkanParticle
{
    alignas(16) glm::vec3 pos;
    alignas(16) glm::vec3 vel;
    alignas(16) glm::vec3 acc;
};

struct VulkanParticlePipelineData
{
    VulkanBuffer data{};
    uint32_t nbParticles{};

    void init(VulkanDevices const &devices, uint32_t nbOfParticles);
    void clear();
};

#endif // PARTICLE_SYS_VULKANPARTICLEPIPELINEDATA_HPP
