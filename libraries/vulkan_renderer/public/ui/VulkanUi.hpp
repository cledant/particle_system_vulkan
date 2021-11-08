#ifndef PARTICLE_SYS_VULKAN_VULKANUI_HPP
#define PARTICLE_SYS_VULKAN_VULKANUI_HPP

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanUiRenderPass.hpp"

class VulkanUi final
{
  public:
    VulkanUi() = default;
    ~VulkanUi() = default;
    VulkanUi(VulkanUi const &src) = delete;
    VulkanUi &operator=(VulkanUi const &rhs) = delete;
    VulkanUi(VulkanUi &&src) = delete;
    VulkanUi &operator=(VulkanUi &&rhs) = delete;

    void init(VulkanInstance const &vkInstance,
              VulkanSwapChain const &swapChain);
    void resize(VulkanSwapChain const &swapChain);
    void clear();

    VkCommandBuffer generateCommandBuffer(uint32_t frameIndex,
                                          VkExtent2D swapChainExtent,
                                          bool noModel = true);

  private:
    VkInstance _instance{};
    VulkanDevices _devices;
    VulkanQueues _queues;

    VulkanUiRenderPass _render_pass;
    VkDescriptorPool _descriptorPool{};
    VkCommandPool _ui_command_pools{};
    std::vector<VkCommandBuffer> _ui_command_buffers;

    inline void _init_imgui(VulkanSwapChain const &swapChain);
    inline void _load_fonts();
    inline void _create_ui_command_buffers(uint32_t nbSwapChainFrames);
};

#endif // PARTICLE_SYS_VULKAN_VULKANUI_HPP
