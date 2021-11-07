#include "ui/VulkanUiRenderPass.hpp"

#include <array>
#include <stdexcept>

#include "interface/AVulkanRenderPassPrivate.hpp"

void
VulkanUiRenderPass::implInit(VulkanInstance const &vkInstance,
                             VulkanSwapChain const &swapChain)
{
    static_cast<void>(vkInstance);
    renderPass = createRenderPass(_devices.device, swapChain, false);
    createFramebuffers(_devices.device, swapChain, renderPass, framebuffers);
    clearRenderPass = createRenderPass(_devices.device, swapChain, true);
    createFramebuffers(
      _devices.device, swapChain, clearRenderPass, clearFramebuffers);
}

void
VulkanUiRenderPass::implResize(VulkanSwapChain const &swapChain)
{
    clean();
    renderPass = createRenderPass(_devices.device, swapChain, false);
    createFramebuffers(_devices.device, swapChain, renderPass, framebuffers);
    clearRenderPass = createRenderPass(_devices.device, swapChain, true);
    createFramebuffers(
      _devices.device, swapChain, clearRenderPass, clearFramebuffers);
}

void
VulkanUiRenderPass::implClean()
{
    for (auto &it : clearFramebuffers) {
        vkDestroyFramebuffer(_devices.device, it, nullptr);
    }
    vkDestroyRenderPass(_devices.device, clearRenderPass, nullptr);
}

void
VulkanUiRenderPass::implClear()
{
    clean();
}

VkRenderPass
VulkanUiRenderPass::createRenderPass(VkDevice device,
                                     VulkanSwapChain const &swapChain,
                                     bool clearPrevious)
{
    // Color
    VkAttachmentDescription color_attachment{};
    color_attachment.format = swapChain.swapChainImageFormat;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = (clearPrevious) ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                              : VK_ATTACHMENT_LOAD_OP_LOAD;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pDepthStencilAttachment = nullptr;

    VkSubpassDependency sub_dep{};
    sub_dep.srcSubpass = VK_SUBPASS_EXTERNAL;
    sub_dep.dstSubpass = 0;
    sub_dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    sub_dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    sub_dep.srcAccessMask = 0;
    sub_dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 1> attachments = {
        color_attachment,
    };
    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = attachments.size();
    render_pass_info.pAttachments = attachments.data();
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &sub_dep;

    VkRenderPass renderPass;
    if (vkCreateRenderPass(device, &render_pass_info, nullptr, &renderPass) !=
        VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanUiRenderPass: failed to create render pass");
    }
    return (renderPass);
}

void
VulkanUiRenderPass::createFramebuffers(VkDevice device,
                                       VulkanSwapChain const &swapChain,
                                       VkRenderPass renderPass,
                                       std::vector<VkFramebuffer> &framebuffers)
{
    framebuffers.resize(swapChain.swapChainImageViews.size());

    size_t i = 0;
    for (auto const &it : swapChain.swapChainImageViews) {
        std::array<VkImageView, 1> sciv = { it };

        framebuffers[i] = createFrameBuffer(
          device, renderPass, sciv, swapChain.swapChainExtent);
        ++i;
    }
}
