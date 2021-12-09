#include "particle/VulkanParticleRenderPass.hpp"

#include "interface/AVulkanRenderPassPrivate.hpp"

void
VulkanParticleRenderPass::implInit(const VulkanInstance &vkInstance,
                                        const VulkanSwapChain &swapChain)
{
    static_cast<void>(vkInstance);
    defaultCreateRenderPass(swapChain,
                            VK_ATTACHMENT_LOAD_OP_LOAD,
                            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    defaultCreateDepthResources(swapChain);
    defaultCreateFramebuffers(swapChain);
}

void
VulkanParticleRenderPass::implResize(const VulkanSwapChain &swapChain)
{
    clean();
    defaultCreateRenderPass(swapChain,
                            VK_ATTACHMENT_LOAD_OP_LOAD,
                            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    defaultCreateDepthResources(swapChain);
    defaultCreateFramebuffers(swapChain);
}

void
VulkanParticleRenderPass::implClean()
{}


void
VulkanParticleRenderPass::implClear()
{
    clean();
}