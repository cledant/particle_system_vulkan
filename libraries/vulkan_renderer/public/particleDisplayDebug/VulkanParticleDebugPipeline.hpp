#ifndef PARTICLE_SYS_VULKANPARTICLEDEBUGPIPELINE_HPP
#define PARTICLE_SYS_VULKANPARTICLEDEBUGPIPELINE_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"
#include "tex/VulkanTextureManager.hpp"
#include "VulkanParticleDebugPipelineData.hpp"
#include "VulkanParticleDebugRenderPass.hpp"

class VulkanParticleDebugPipeline final
{
  public:
    VulkanParticleDebugPipeline() = default;
    ~VulkanParticleDebugPipeline() = default;
    VulkanParticleDebugPipeline(VulkanParticleDebugPipeline const &src) =
      delete;
    VulkanParticleDebugPipeline &operator=(
      VulkanParticleDebugPipeline const &rhs) = delete;
    VulkanParticleDebugPipeline(VulkanParticleDebugPipeline &&src) = delete;
    VulkanParticleDebugPipeline &operator=(VulkanParticleDebugPipeline &&rhs) =
      delete;

    void init(VulkanInstance const &vkInstance,
              VulkanSwapChain const &swapChain,
              uint64_t nbParticles,
              VkBuffer systemUbo);
    void resize(VulkanSwapChain const &swapChain, VkBuffer systemUbo);
    void clear();

    void setParticleNumber(uint64_t nbParticles);
    void setParticlesColor(glm::vec3 const &particlesColor);
    void setParticleGravityCenter(glm::vec3 const &particleGravityCenter);
    void setUniformOnGpu(uint32_t currentImg);

    [[nodiscard]] VulkanParticleDebugRenderPass const &getRenderPass() const;
    void generateCommands(VkCommandBuffer cmdBuffer, size_t descriptorSetIndex);

  private:
    // Vulkan related
    VkDevice _device{};
    VkPhysicalDevice _physical_device{};
    VkCommandPool _cmd_pool{};
    VkQueue _gfx_queue{};
    VkDescriptorSetLayout _descriptor_set_layout{};
    VkPipelineLayout _pipeline_layout{};
    VkPipeline _graphic_pipeline{};
    VulkanParticleDebugPipelineData _pipeline_data;
    VulkanParticleDebugRenderPass _pipeline_render_pass;

    // Particle related
    VkBuffer _particle_uniform{};
    VkDeviceMemory _particle_uniform_memory{};
    glm::vec3 _particles_gravity_center{};
    glm::vec3 _particles_color{};

    inline void _create_descriptor_layout();
    inline void _create_pipeline_layout();
    inline void _create_gfx_pipeline(VulkanSwapChain const &swapChain);
    inline VulkanParticleDebugPipelineData _create_pipeline_particle_debug(
      uint64_t nbParticles);
    inline void _update_pipeline_particle_debug(uint64_t nbParticles);
    inline void _create_descriptor_pool(
      VulkanSwapChain const &swapChain,
      VulkanParticleDebugPipelineData &pipelineData);
    inline void _create_descriptor_sets(
      VulkanSwapChain const &swapChain,
      VulkanParticleDebugPipelineData &pipelineData,
      VkBuffer systemUbo);
    inline void _create_particle_debug_uniform_buffer(
      uint32_t currentSwapChainNbImg);
    inline void _generate_particles();
};

#endif // PARTICLE_SYS_VULKANPARTICLEDEBUGPIPELINE_HPP
