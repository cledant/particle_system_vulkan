#ifndef PARTICLE_SYSTEM_VULKAN_VULKANFRAMEBUFFER_HPP
#define PARTICLE_SYSTEM_VULKAN_VULKANFRAMEBUFFER_HPP

#include <array>
#include <stdexcept>

#include <vulkan/vulkan.h>

template<std::size_t SIZE>
VkFramebuffer
createFrameBuffer(VkDevice device,
                  VkRenderPass renderPass,
                  std::array<VkImageView, SIZE> imgViews,
                  VkExtent2D extent)
{
    VkFramebufferCreateInfo framebuffer_info{};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = renderPass;
    framebuffer_info.attachmentCount = imgViews.size();
    framebuffer_info.pAttachments = imgViews.data();
    framebuffer_info.width = extent.width;
    framebuffer_info.height = extent.height;
    framebuffer_info.layers = 1;

    VkFramebuffer framebuffer;
    if (vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create framebuffer");
    }
    return (framebuffer);
}

#endif // PARTICLE_SYSTEM_VULKAN_VULKANFRAMEBUFFER_HPP
