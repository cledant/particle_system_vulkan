#ifndef PARTICLE_SYSTEM_VULKAN_AVULKANRENDERPASS_HPP
#define PARTICLE_SYSTEM_VULKAN_AVULKANRENDERPASS_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"

template<class Child>
class AVulkanRenderPass
{
  public:
    AVulkanRenderPass() = default;
    virtual ~AVulkanRenderPass() = default;
    AVulkanRenderPass(AVulkanRenderPass const &src) = default;
    AVulkanRenderPass &operator=(AVulkanRenderPass const &rhs) = default;
    AVulkanRenderPass(AVulkanRenderPass &&src) noexcept = default;
    AVulkanRenderPass &operator=(AVulkanRenderPass &&rhs) noexcept = default;
    void init(VulkanInstance const &vkInstance,
              VulkanSwapChain const &swapChain);
    void resize(VulkanSwapChain const &swapChain);
    void clean();
    void clear();

    std::vector<VkFramebuffer> framebuffers;
    VulkanTexture depthTex{};
    VkRenderPass renderPass{};

  protected:
    VulkanDevices _devices;
    VulkanQueues _queues;
    VulkanCommandPools _cmdPools;

    inline void defaultCreateRenderPass(VulkanSwapChain const &swapChain);
    inline void defaultCreateDepthResources(VulkanSwapChain const &swapChain);
    inline void defaultCreateFramebuffers(VulkanSwapChain const &swapChain);
};

template<class Child>
void
AVulkanRenderPass<Child>::init(VulkanInstance const &vkInstance,
                               VulkanSwapChain const &swapChain)
{
    _devices = vkInstance.devices;
    _queues = vkInstance.queues;
    _cmdPools = vkInstance.cmdPools;
    static_cast<Child &>(*this).implInit(vkInstance, swapChain);
}

template<class Child>
void
AVulkanRenderPass<Child>::resize(VulkanSwapChain const &swapChain)
{
    static_cast<Child &>(*this).implResize(swapChain);
}

template<class Child>
void
AVulkanRenderPass<Child>::clear()
{
    static_cast<Child &>(*this).implClear();
    framebuffers.clear();
    depthTex = VulkanTexture{};
    renderPass = nullptr;
    _devices = VulkanDevices{};
    _queues = VulkanQueues{};
    _cmdPools = VulkanCommandPools{};
}

template<class Child>
void
AVulkanRenderPass<Child>::clean()
{
    static_cast<Child &>(*this).implClean();
    depthTex.clear();
    for (auto &it : framebuffers) {
        vkDestroyFramebuffer(_devices.device, it, nullptr);
    }
    vkDestroyRenderPass(_devices.device, renderPass, nullptr);
}

#endif // PARTICLE_SYSTEM_VULKAN_AVULKANRENDERPASS_HPP
