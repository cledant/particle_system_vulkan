#ifndef PARTICLE_SYS_VULKANPARTICLEPIPELINE_HPP
#define PARTICLE_SYS_VULKANPARTICLEPIPELINE_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"
#include "tex/VulkanTextureManager.hpp"
#include "ubo/VulkanUboStructs.hpp"
#include "renderPass/VulkanSceneRenderPass.hpp"
#include "VulkanParticlePipelineData.hpp"
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
              VulkanSceneRenderPass const &renderPass,
              uint32_t nbParticles,
              uint32_t maxSpeedParticle,
              glm::vec3 const &particlesColor,
              float particleMass,
              VkBuffer systemUbo);
    void resize(VulkanSwapChain const &swapChain,
                VulkanSceneRenderPass const &renderPass,
                VkBuffer systemUbo);
    void clear();

    void setParticleNumber(uint32_t nbParticles,
                           VulkanSwapChain const &swapChain,
                           VkBuffer systemUbo);
    void setParticleMaxSpeed(uint32_t maxSpeed);
    void setParticlesColor(glm::vec3 const &particlesColor);
    void setParticleGravityCenter(glm::vec3 const &particleGravityCenter);
    void setParticleMass(float mass);
    void setDeltaT(float deltaT);
    void setGfxUboOnGpu(uint32_t currentImg);
    void setCompUboOnGpu();

    void generateCommands(VkCommandBuffer cmdBuffer, size_t descriptorSetIndex);
    void generateComputeCommands(VkCommandBuffer cmdBuffer,
                                 VulkanParticleComputeShaderType type);
    void acquireComputeBufferBarrier(VkCommandBuffer &cmdBuffer) const;
    void releaseComputeBufferBarrier(VkCommandBuffer &cmdBuffer) const;

  private:
    // Vulkan related
    VulkanDevices _devices;
    VulkanQueues _queues;
    VulkanCommandPools _cmdPools;

    // Global
    VkDescriptorPool _descriptorPool{};
    VulkanParticlePipelineData _pipelineData;

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
    static constexpr float const DEFAULT_PARTICLE_MASS = 5.0f;

    inline void createGfxPipeline(VulkanSwapChain const &swapChain,
                                  VulkanSceneRenderPass const &renderPass);
    inline void createDescriptorPool(uint32_t descriptorCount);
    inline void createGfxDescriptorSets(VkBuffer systemUbo,
                                        uint32_t descriptorCount);
    inline void generateRandomSeed();
    inline void createComputePipeline();
    inline void createComputeDescriptorSets();
};

#endif // PARTICLE_SYS_VULKANPARTICLEPIPELINE_HPP
