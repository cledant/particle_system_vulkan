#include "skybox/VulkanSkyboxRenderPass.hpp"

#include "interface/AVulkanRenderPassPrivate.hpp"

void
VulkanSkyboxRenderPass::implInit(VulkanInstance const &vkInstance,
                                 VulkanSwapChain const &swapChain)
{
    static_cast<void>(vkInstance);
    defaultCreateRenderPass(swapChain);
    defaultCreateDepthResources(swapChain);
    defaultCreateFramebuffers(swapChain);
}

void
VulkanSkyboxRenderPass::implResize(VulkanSwapChain const &swapChain)
{
    clean();
    defaultCreateRenderPass(swapChain);
    defaultCreateDepthResources(swapChain);
    defaultCreateFramebuffers(swapChain);
}

void
VulkanSkyboxRenderPass::implClean()
{}

void
VulkanSkyboxRenderPass::implClear()
{
    clean();
}