#ifndef PARTICLE_SYS_VULKANPARTICLERENDERPASS_HPP
#define PARTICLE_SYS_VULKANPARTICLERENDERPASS_HPP

#include <vulkan/vulkan.h>

#include "interface/AVulkanRenderPass.hpp"

class VulkanParticleRenderPass final
  : public AVulkanRenderPass<VulkanParticleRenderPass>
{
  public:
    VulkanParticleRenderPass() = default;
    ~VulkanParticleRenderPass() override = default;
    VulkanParticleRenderPass(VulkanParticleRenderPass const &src) =
      default;
    VulkanParticleRenderPass &operator=(VulkanParticleRenderPass const &rhs) = default;
    VulkanParticleRenderPass(VulkanParticleRenderPass &&src) noexcept = default;
    VulkanParticleRenderPass &operator=(
      VulkanParticleRenderPass &&rhs) noexcept = default;

    void implInit(VulkanInstance const &vkInstance,
                  VulkanSwapChain const &swapChain);
    void implResize(VulkanSwapChain const &swapChain);
    void implClean();
    void implClear();
};

#endif // PARTICLE_SYS_VULKANPARTICLERENDERPASS_HPP
