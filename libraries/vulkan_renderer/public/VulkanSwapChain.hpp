#ifndef PARTICLE_SYS_VULKAN_VULKANSWAPCHAIN_HPP
#define PARTICLE_SYS_VULKAN_VULKANSWAPCHAIN_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "VulkanInstance.hpp"

class VulkanSwapChain final
{
  public:
    VulkanSwapChain() = default;
    ~VulkanSwapChain() = default;
    VulkanSwapChain(VulkanSwapChain const &src) = delete;
    VulkanSwapChain &operator=(VulkanSwapChain const &rhs) = delete;
    VulkanSwapChain(VulkanSwapChain &&src) = delete;
    VulkanSwapChain &operator=(VulkanSwapChain &&rhs) = delete;

    void init(VulkanInstance const &vkInstance, uint32_t fb_w, uint32_t fb_h);
    void resize(uint32_t fb_w, uint32_t fb_h);
    void clean();
    void clear();

    uint32_t oldSwapChainNbImg{};
    uint32_t currentSwapChainNbImg{};
    VkSwapchainKHR swapChain{};
    VkFormat swapChainImageFormat{};
    VkExtent2D swapChainExtent{};
    std::vector<VkImage> swapChainImages;
    std::vector<VulkanTexture> swapChainImageViews;

  private:
    VulkanDevices _devices{};
    VkSurfaceKHR _surface{};

    inline void _create_swap_chain(uint32_t fb_w, uint32_t fb_h);
    inline void _create_image_view();
};

#endif // PARTICLE_SYS_VULKAN_VULKANSWAPCHAIN_HPP
