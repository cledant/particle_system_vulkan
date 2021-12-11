#include "renderPass//VulkanSceneRenderPass.hpp"

#include "interface/AVulkanRenderPassPrivate.hpp"

void
VulkanSceneRenderPass::implInit(VulkanInstance const &vkInstance,
                                VulkanSwapChain const &swapChain)
{
    static_cast<void>(vkInstance);
    defaultCreateRenderPass(swapChain,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    defaultCreateDepthResources(swapChain);
    defaultCreateFramebuffers(swapChain);
}

void
VulkanSceneRenderPass::implResize(VulkanSwapChain const &swapChain)
{
    clean();
    defaultCreateRenderPass(swapChain,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    defaultCreateDepthResources(swapChain);
    defaultCreateFramebuffers(swapChain);
}

void
VulkanSceneRenderPass::implClean()
{}

void
VulkanSceneRenderPass::implClear()
{
    clean();
}