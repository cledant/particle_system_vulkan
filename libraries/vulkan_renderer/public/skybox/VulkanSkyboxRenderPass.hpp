#ifndef PARTICLE_SYS_VULKANSKYBOXRENDERPASS_HPP
#define PARTICLE_SYS_VULKANSKYBOXRENDERPASS_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"

class VulkanSkyboxRenderPass final
{
  public:
    VulkanSkyboxRenderPass() = default;
    ~VulkanSkyboxRenderPass() = default;
    VulkanSkyboxRenderPass(VulkanSkyboxRenderPass const &src) = delete;
    VulkanSkyboxRenderPass &operator=(VulkanSkyboxRenderPass const &rhs) = delete;
    VulkanSkyboxRenderPass(VulkanSkyboxRenderPass &&src) = delete;
    VulkanSkyboxRenderPass &operator=(VulkanSkyboxRenderPass &&rhs) = delete;

    void init(VulkanInstance const &vkInstance,
              VulkanSwapChain const &swapChain);
    void resize(VulkanSwapChain const &swapChain);
    void clear();

    std::vector<VkFramebuffer> framebuffers;
    VkFormat depthFormat{};
    VkImage depthImage{};
    VkDeviceMemory depthImgMemory{};
    VkImageView depthImgView{};
    VkRenderPass renderPass{};

  private:
    VkDevice _device{};
    VkPhysicalDevice _physical_device{};
    VkCommandPool _command_pool{};
    VkQueue _gfx_queue{};

    inline void _create_render_pass(VulkanSwapChain const &swapChain);
    inline void _create_depth_resources(VulkanSwapChain const &swapChain);
    inline void _create_framebuffers(VulkanSwapChain const &swapChain);
};

#endif // PARTICLE_SYS_VULKANSKYBOXRENDERPASS_HPP
