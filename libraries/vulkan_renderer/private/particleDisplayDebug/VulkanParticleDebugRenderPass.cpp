#include "particleDisplayDebug//VulkanParticleDebugRenderPass.hpp"

#include "interface/AVulkanRenderPassPrivate.hpp"

void
VulkanParticleDebugRenderPass::implInit(const VulkanInstance &vkInstance,
                                        const VulkanSwapChain &swapChain)
{
    static_cast<void>(vkInstance);
    defaultCreateRenderPass(swapChain);
    defaultCreateDepthResources(swapChain);
    defaultCreateFramebuffers(swapChain);
}

void
VulkanParticleDebugRenderPass::implResize(const VulkanSwapChain &swapChain)
{
    clean();
    defaultCreateRenderPass(swapChain);
    defaultCreateDepthResources(swapChain);
    defaultCreateFramebuffers(swapChain);
}

void
VulkanParticleDebugRenderPass::implClean()
{}


void
VulkanParticleDebugRenderPass::implClear()
{
    clean();
}