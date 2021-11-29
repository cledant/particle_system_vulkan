#ifndef PARTICLE_SYS_VULKANPARTICLEPIPELINE_HPP
#define PARTICLE_SYS_VULKANPARTICLEPIPELINE_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"
#include "tex/VulkanTextureManager.hpp"
#include "ubo/VulkanUboStructs.hpp"
#include "VulkanParticlePipelineData.hpp"
#include "VulkanParticleRenderPass.hpp"
#include "VulkanParticleGfxPipelineDescription.hpp"
#include "VulkanParticleComputePipelineDescription.hpp"

enum VulkanParticleComputeShaderType
{
    VPCST_RANDOM_CUBE = 0,
    VPCST_RANDOM_SPHERE,
    VPCST_RANDOM_DISK,
    VPCST_GRAVITY,
    VPCST_NB,
};

class VulkanParticlePipeline final
{
  public:
    VulkanParticlePipeline() = default;
    ~VulkanParticlePipeline() = default;
    VulkanParticlePipeline(VulkanParticlePipeline const &src) = delete;
    VulkanParticlePipeline &operator=(VulkanParticlePipeline const &rhs) =
      delete;
    VulkanParticlePipeline(VulkanParticlePipeline &&src) = delete;
    VulkanParticlePipeline &operator=(VulkanParticlePipeline &&rhs) = delete;

    void init(VulkanInstance const &vkInstance,
              VulkanSwapChain const &swapChain,
              uint32_t nbParticles,
              glm::vec3 const &particles_color,
              VkBuffer systemUbo);
    void resize(VulkanSwapChain const &swapChain, VkBuffer systemUbo);
    void clear();

    void setParticleNumber(uint32_t nbParticles,
                           VulkanSwapChain const &swapChain,
                           VkBuffer systemUbo);
    void setParticlesColor(glm::vec3 const &particlesColor);
    void setParticleGravityCenter(glm::vec3 const &particleGravityCenter);
    void setDeltaT(float deltaT);
    void setGfxUboOnGpu(uint32_t currentImg);
    void setCompUboOnGpu();

    [[nodiscard]] VulkanParticleRenderPass const &getRenderPass() const;
    void generateCommands(VkCommandBuffer cmdBuffer, size_t descriptorSetIndex);
    void generateComputeCommands(VkCommandBuffer cmdBuffer,
                                 VulkanParticleComputeShaderType type);

  private:
    // Vulkan related
    VulkanDevices _devices;
    VulkanQueues _queues;
    VulkanCommandPools _cmdPools;

    // Global
    VkDescriptorPool _descriptorPool{};
    VulkanParticlePipelineData _pipelineData;
    VulkanParticleRenderPass _renderPass;

    // Vertex / Fragment shaders related
    VkPipeline _gfxPipeline{};
    VulkanParticleGfxPipelineDescription _gfxDescription;
    std::vector<VkDescriptorSet> _gfxDescriptorSets;
    VulkanBuffer _gfxUniform;
    ParticleGfxUbo _gfxUbo{};

    // Compute shaders related
    VulkanBuffer _computeUniform;
    VulkanParticleComputePipelineDescription _computeDescription;
    std::vector<VkDescriptorSet> _computeDescriptorSet{};
    std::array<VkPipeline, VPCST_NB> _compShaders{};
    ParticleComputeUbo _compUbo{};

    static constexpr std::array<char const *, VPCST_NB> const
      COMPUTE_SHADER_PATH = {
          "resources/shaders/particle/particleRandomCube.comp.spv",
          "resources/shaders/particle/particleRandomSphere.comp.spv",
          "resources/shaders/particle/particleRandomDisk.comp.spv",
          "resources/shaders/particle/particleGravity.comp.spv"
      };

    inline void createGfxPipeline(VulkanSwapChain const &swapChain);
    inline void createDescriptorPool(uint32_t descriptorCount);
    inline void createGfxDescriptorSets(VkBuffer systemUbo,
                                        uint32_t descriptorCount);
    inline void generateRandomSeed();
    inline void createComputePipeline();
    inline void createComputeDescriptorSets();
};

#endif // PARTICLE_SYS_VULKANPARTICLEPIPELINE_HPP
