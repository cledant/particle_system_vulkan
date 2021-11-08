#ifndef PARTICLE_SYS_VULKAN_VULKANUIRENDERPASS_HPP
#define PARTICLE_SYS_VULKAN_VULKANUIRENDERPASS_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "interface/AVulkanRenderPass.hpp"

class VulkanUiRenderPass final : public AVulkanRenderPass<VulkanUiRenderPass>
{
  public:
    VulkanUiRenderPass() = default;
    ~VulkanUiRenderPass() override = default;
    VulkanUiRenderPass(VulkanUiRenderPass const &src) = default;
    VulkanUiRenderPass &operator=(VulkanUiRenderPass const &rhs) = default;
    VulkanUiRenderPass(VulkanUiRenderPass &&src) noexcept = default;
    VulkanUiRenderPass &operator=(VulkanUiRenderPass &&rhs) noexcept = default;

    void implInit(VulkanInstance const &vkInstance,
                  VulkanSwapChain const &swapChain);
    void implResize(VulkanSwapChain const &swapChain);
    void implClean();
    void implClear();

    std::vector<VkFramebuffer> clearFramebuffers;
    VkRenderPass clearRenderPass{};

  private:
    inline static VkRenderPass createRenderPass(
      VkDevice device,
      VulkanSwapChain const &swapChain,
      bool clearPrevious);
    inline static void createFramebuffers(
      VkDevice device,
      VulkanSwapChain const &swapChain,
      VkRenderPass renderPass,
      std::vector<VkFramebuffer> &framebuffers);
};

#endif // PARTICLE_SYS_VULKAN_VULKANUIRENDERPASS_HPP
