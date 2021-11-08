#ifndef PARTICLE_SYS_VULKANPARTICLEDEBUGRENDERPASS_HPP
#define PARTICLE_SYS_VULKANPARTICLEDEBUGRENDERPASS_HPP

#include <vulkan/vulkan.h>

#include "interface/AVulkanRenderPass.hpp"

class VulkanParticleDebugRenderPass final
  : public AVulkanRenderPass<VulkanParticleDebugRenderPass>
{
  public:
    VulkanParticleDebugRenderPass() = default;
    ~VulkanParticleDebugRenderPass() override = default;
    VulkanParticleDebugRenderPass(VulkanParticleDebugRenderPass const &src) =
      default;
    VulkanParticleDebugRenderPass &operator=(
      VulkanParticleDebugRenderPass const &rhs) = default;
    VulkanParticleDebugRenderPass(
      VulkanParticleDebugRenderPass &&src) noexcept = default;
    VulkanParticleDebugRenderPass &operator=(
      VulkanParticleDebugRenderPass &&rhs) noexcept = default;

    void implInit(VulkanInstance const &vkInstance,
                  VulkanSwapChain const &swapChain);
    void implResize(VulkanSwapChain const &swapChain);
    void implClean();
    void implClear();
};

#endif // PARTICLE_SYS_VULKANPARTICLEDEBUGRENDERPASS_HPP
