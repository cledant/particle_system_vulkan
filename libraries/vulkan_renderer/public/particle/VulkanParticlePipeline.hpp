#ifndef PARTICLE_SYS_VULKANPARTICLEPIPELINE_HPP
#define PARTICLE_SYS_VULKANPARTICLEPIPELINE_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"
#include "tex/VulkanTextureManager.hpp"
#include "VulkanParticlePipelineData.hpp"
#include "VulkanParticleRenderPass.hpp"
#include "VulkanParticlePipelineDescription.hpp"

class VulkanParticlePipeline final
{
  public:
    VulkanParticlePipeline() = default;
    ~VulkanParticlePipeline() = default;
    VulkanParticlePipeline(VulkanParticlePipeline const &src) =
      delete;
    VulkanParticlePipeline &operator=(VulkanParticlePipeline const &rhs) = delete;
    VulkanParticlePipeline(VulkanParticlePipeline &&src) = delete;
    VulkanParticlePipeline &operator=(VulkanParticlePipeline &&rhs) =
      delete;

    void init(VulkanInstance const &vkInstance,
              VulkanSwapChain const &swapChain,
              uint64_t nbParticles,
              glm::vec3 const &particles_color,
              VkBuffer systemUbo);
    void resize(VulkanSwapChain const &swapChain, VkBuffer systemUbo);
    void clear();

    void setParticleNumber(uint64_t nbParticles,
                           VulkanSwapChain const &swapChain,
                           VkBuffer systemUbo);
    void setParticlesColor(glm::vec3 const &particlesColor);
    void setParticleGravityCenter(glm::vec3 const &particleGravityCenter);
    void setUniformOnGpu(uint32_t currentImg);

    [[nodiscard]] VulkanParticleRenderPass const &getRenderPass() const;
    void generateCommands(VkCommandBuffer cmdBuffer, size_t descriptorSetIndex);
    void generateComputeCommands(VkCommandBuffer cmdBuffer);

  private:
    // Vulkan related
    VulkanDevices _devices;
    VulkanQueues _queues;
    VulkanCommandPools _cmdPools;

    // Vertex / Fragment shader related
    VkDescriptorSetLayout _descriptor_set_layout{};
    VkPipelineLayout _pipeline_layout{};
    VulkanParticlePipelineData _pipeline_data;
    VulkanParticleRenderPass _pipeline_render_pass;
    VkBuffer _particle_uniform{};
    VkDeviceMemory _particle_uniform_memory{};
    glm::vec3 _particles_gravity_center{};
    glm::vec3 _particles_color{};

    // Compute shader related
    static constexpr uint32_t const DEFAULT_COMPUTE_WORK_GROUP_SIZE = 256;
    uint32_t _compute_work_group_size{};
    VkDescriptorSetLayout _compute_descriptor_set_layout{};
    VkPipelineLayout _compute_pipeline_layout{};
    VkBuffer _particle_compute_uniform{};
    VkDeviceMemory _particle_compute_uniform_memory{};

    // Global
    //VkDescriptorPool _globalDescriptorPool{};
    VkPipeline _graphic_pipeline{};
    VkPipeline _compute_pipeline{};

    inline void _create_descriptor_layout();
    inline void _create_pipeline_layout();
    inline void _create_gfx_pipeline(VulkanSwapChain const &swapChain);
    inline VulkanParticlePipelineData _create_pipeline_particle_debug(
      uint64_t nbParticles);
    inline void _reallocate_pipeline_particle_debug_buffers(
      uint64_t nbParticles);
    inline void _create_descriptor_pool(
      VulkanSwapChain const &swapChain,
      VulkanParticlePipelineData &pipelineData);
    inline void _create_descriptor_sets(
      VulkanSwapChain const &swapChain,
      VulkanParticlePipelineData &pipelineData,
      VkBuffer systemUbo);
    inline void _create_particle_debug_uniform_buffer(
      uint32_t currentSwapChainNbImg);
    inline void _generate_particles();

    inline void _create_particle_compute_debug_uniform_buffer();
    inline void _create_compute_descriptor_layout();
    inline void _create_compute_pipeline_layout();
    inline void _create_compute_pipeline();
    inline void _create_compute_descriptor_sets(
      VulkanParticlePipelineData &pipelineData);
};

#endif // PARTICLE_SYS_VULKANPARTICLEPIPELINE_HPP
