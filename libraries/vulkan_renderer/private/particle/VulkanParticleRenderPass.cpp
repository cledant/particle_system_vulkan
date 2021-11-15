#include "particle/VulkanParticleRenderPass.hpp"

#include "interface/AVulkanRenderPassPrivate.hpp"

void
VulkanParticleRenderPass::implInit(const VulkanInstance &vkInstance,
                                        const VulkanSwapChain &swapChain)
{
    static_cast<void>(vkInstance);
    defaultCreateRenderPass(swapChain);
    defaultCreateDepthResources(swapChain);
    defaultCreateFramebuffers(swapChain);
}

void
VulkanParticleRenderPass::implResize(const VulkanSwapChain &swapChain)
{
    clean();
    defaultCreateRenderPass(swapChain);
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