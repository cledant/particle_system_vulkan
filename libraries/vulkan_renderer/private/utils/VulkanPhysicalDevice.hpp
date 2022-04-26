#ifndef PARTICLE_SYS_VULKAN_VULKANPHYSICALDEVICE_HPP
#define PARTICLE_SYS_VULKAN_VULKANPHYSICALDEVICE_HPP

#include <cstdint>
#include <vector>
#include <optional>
#include <array>

#include <vulkan/vulkan.h>

struct DeviceRequirement final
{
    std::optional<uint32_t> graphic_family_index;
    std::optional<uint32_t> present_family_index;
    std::optional<uint32_t> compute_family_index;
    VkBool32 geometry_shader{};
    VkBool32 sampler_aniso{};
    VkBool32 fill_mode_non_solid{};
    VkBool32 all_extension_supported{};

    [[nodiscard]] bool isValid() const;
};

[[maybe_unused]] constexpr std::array const DEVICE_EXTENSIONS{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

VkPhysicalDevice selectBestDevice(std::vector<VkPhysicalDevice> const &devices,
                                  VkSurfaceKHR surface);
int rateDevice(VkPhysicalDevice device, VkSurfaceKHR surface);
char *getDeviceName(char *dst, VkPhysicalDevice device);
DeviceRequirement getDeviceRequirement(VkPhysicalDevice device,
                                       VkSurfaceKHR surface);
void checkDeviceFeaturesSupport(VkPhysicalDevice device, DeviceRequirement &dr);
void checkDeviceExtensionSupport(VkPhysicalDevice device,
                                 DeviceRequirement &dr);
void getDeviceQueues(VkPhysicalDevice device,
                     VkSurfaceKHR surface,
                     DeviceRequirement &dr);
VkDeviceSize getMinUniformBufferOffsetAlignment(VkPhysicalDevice device);
bool getLinearBlittingSupport(VkPhysicalDevice device, VkFormat imgFormat);
VkPhysicalDeviceProperties getPhysicalDeviceProperties(VkPhysicalDevice device);

#endif // PARTICLE_SYS_VULKAN_VULKANPHYSICALDEVICE_HPP
