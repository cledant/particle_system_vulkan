#ifndef PARTICLE_SYS_VULKANSKYBOXRENDERPASS_HPP
#define PARTICLE_SYS_VULKANSKYBOXRENDERPASS_HPP

#include <vulkan/vulkan.h>

#include "interface/AVulkanRenderPass.hpp"

class VulkanSkyboxRenderPass final
  : public AVulkanRenderPass<VulkanSkyboxRenderPass>
{
  public:
    VulkanSkyboxRenderPass() = default;
    ~VulkanSkyboxRenderPass() override = default;
    VulkanSkyboxRenderPass(VulkanSkyboxRenderPass const &src) = default;
    VulkanSkyboxRenderPass &operator=(VulkanSkyboxRenderPass const &rhs) =
      default;
    VulkanSkyboxRenderPass(VulkanSkyboxRenderPass &&src) noexcept = default;
    VulkanSkyboxRenderPass &operator=(VulkanSkyboxRenderPass &&rhs) noexcept =
      default;

    void implInit(VulkanInstance const &vkInstance,
                  VulkanSwapChain const &swapChain);
    void implResize(VulkanSwapChain const &swapChain);
    void implClean();
    void implClear();
};

#endif // PARTICLE_SYS_VULKANSKYBOXRENDERPASS_HPP
