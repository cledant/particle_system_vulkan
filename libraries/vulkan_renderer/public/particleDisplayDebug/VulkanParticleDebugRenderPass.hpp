#ifndef PARTICLE_SYS_VULKANPARTICLEDEBUGRENDERPASS_HPP
#define PARTICLE_SYS_VULKANPARTICLEDEBUGRENDERPASS_HPP

#include <vulkan/vulkan.h>

#include "interface/AVulkanRenderPass.hpp"

class VulkanParticleDebugRenderPass final
  : public AVulkanRenderPass<VulkanParticleDebugRenderPass>
{
  public:
    VulkanParticleDebugRenderPass() = default;
    ~VulkanParticleDebugRenderPass() = default;
    VulkanParticleDebugRenderPass(VulkanParticleDebugRenderPass const &src) =
      delete;
    VulkanParticleDebugRenderPass &operator=(
      VulkanParticleDebugRenderPass const &rhs) = delete;
    VulkanParticleDebugRenderPass(VulkanParticleDebugRenderPass &&src) = delete;
    VulkanParticleDebugRenderPass &operator=(
      VulkanParticleDebugRenderPass &&rhs) = delete;

    void implInit(VulkanInstance const &vkInstance,
                  VulkanSwapChain const &swapChain);
    void implResize(VulkanSwapChain const &swapChain);
    void implClean();
    void implClear();
};

#endif // PARTICLE_SYS_VULKANPARTICLEDEBUGRENDERPASS_HPP
