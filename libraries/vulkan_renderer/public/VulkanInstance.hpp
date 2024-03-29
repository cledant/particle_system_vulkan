#ifndef PARTICLE_SYS_VULKAN_VULKANINSTANCE_HPP
#define PARTICLE_SYS_VULKAN_VULKANINSTANCE_HPP

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include "common/VulkanCommonStruct.hpp"

class VulkanInstance final
{
  public:
    VulkanInstance() = default;
    ~VulkanInstance() = default;
    VulkanInstance(VulkanInstance const &src) = delete;
    VulkanInstance &operator=(VulkanInstance const &rhs) = delete;
    VulkanInstance(VulkanInstance &&src) = delete;
    VulkanInstance &operator=(VulkanInstance &&rhs) = delete;

    static VkInstance createInstance(
      std::string const &app_name,
      std::string const &engine_name,
      uint32_t app_version,
      uint32_t engine_version,
      std::vector<char const *> &&required_extensions);
    void init(VkSurfaceKHR windowSurface);
    void clear();

    VkInstance instance{};
    VkSurfaceKHR surface{};
    VkDebugUtilsMessengerEXT debugMessenger{};

    VulkanDevices devices;
    char deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE]{};

    VulkanQueues queues;
    VulkanCommandPools cmdPools;

  private:
    inline void _setup_vk_debug_msg();
    inline void _select_physical_device();
    inline void _create_queues();

    // Dbg related
    static inline bool _check_validation_layer_support();
};

#endif // PARTICLE_SYS_VULKAN_VULKANINSTANCE_HPP
