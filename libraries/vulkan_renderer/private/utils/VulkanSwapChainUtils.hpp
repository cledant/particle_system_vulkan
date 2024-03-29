#ifndef PARTICLE_SYS_VULKAN_VULKANSWAPCHAINUTILS_HPP
#define PARTICLE_SYS_VULKAN_VULKANSWAPCHAINUTILS_HPP

#include <vector>
#include <optional>

#include <vulkan/vulkan.h>

struct SwapChainSupportInfo final
{
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_mode;
};

struct SwapChainSupport final
{
    VkSurfaceCapabilitiesKHR capabilities{};
    std::optional<VkSurfaceFormatKHR> surface_format;
    std::optional<VkPresentModeKHR> present_mode;
    VkExtent2D extent{};

    [[nodiscard]] bool isValid() const;
};

SwapChainSupport getSwapChainSupport(VkPhysicalDevice device,
                                     VkSurfaceKHR surface,
                                     VkExtent2D actual_extent);
bool checkSwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
SwapChainSupportInfo getSwapChainSupportInfo(VkPhysicalDevice device,
                                             VkSurfaceKHR surface);
std::optional<VkSurfaceFormatKHR> getSwapChainSurfaceFormat(
  std::vector<VkSurfaceFormatKHR> const &available_formats);
std::optional<VkPresentModeKHR> getSwapChainPresentMode(
  std::vector<VkPresentModeKHR> const &available_present_mode);
VkExtent2D getSwapChainExtent(VkSurfaceCapabilitiesKHR const &capabilities,
                              VkExtent2D actual_extent);

#endif // PARTICLE_SYS_VULKAN_VULKANSWAPCHAINUTILS_HPP
