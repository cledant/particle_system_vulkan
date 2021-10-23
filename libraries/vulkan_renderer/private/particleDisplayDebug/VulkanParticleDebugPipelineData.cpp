#include "particleDisplayDebug/VulkanParticleDebugPipelineData.hpp"

std::array<VkVertexInputBindingDescription, 1>
VulkanParticleDebugPipelineData::getInputBindingDescription()
{
    std::array<VkVertexInputBindingDescription, 1> binding_description{};

    binding_description[0].binding = 0;
    binding_description[0].stride = sizeof(glm::vec3);
    binding_description[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return (binding_description);
}

std::array<VkVertexInputAttributeDescription, 1>
VulkanParticleDebugPipelineData::getInputAttributeDescription()
{
    std::array<VkVertexInputAttributeDescription, 1> attribute_description{};

    attribute_description[0].binding = 0;
    attribute_description[0].location = 0;
    attribute_description[0].offset = 0;
    attribute_description[0].format = VK_FORMAT_R32G32B32_SFLOAT;

    return (attribute_description);
}

void
VulkanParticleDebugPipelineData::clear()
{
    buffer = nullptr;
    memory = nullptr;
    descriptorPool = nullptr;
    nbParticles = 0;
    particleBufferSize = 0;
    descriptorSets.clear();
}
