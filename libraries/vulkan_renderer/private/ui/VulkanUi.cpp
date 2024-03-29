#include "ui/VulkanUi.hpp"

#include <stdexcept>
#include <array>

#include "utils/VulkanCommandBuffer.hpp"

void
VulkanUi::init(VulkanInstance const &vkInstance,
               VulkanSwapChain const &swapChain)
{
    _instance = vkInstance.instance;
    _devices = vkInstance.devices;
    _queues = vkInstance.queues;
    _render_pass.init(vkInstance, swapChain);
    _init_imgui(swapChain);
    _ui_command_pools =
      createCommandPool(_devices.device,
                        _queues.graphicFamilyIndex,
                        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    _create_ui_command_buffers(swapChain.currentSwapChainNbImg);
    _load_fonts();
}

void
VulkanUi::resize(VulkanSwapChain const &swapChain)
{
    vkDeviceWaitIdle(_devices.device);
    ImGui_ImplVulkan_Shutdown();
    _render_pass.resize(swapChain);
    vkDestroyCommandPool(_devices.device, _ui_command_pools, nullptr);
    vkDestroyDescriptorPool(_devices.device, _descriptorPool, nullptr);
    _init_imgui(swapChain);
    _ui_command_pools =
      createCommandPool(_devices.device,
                        _queues.graphicFamilyIndex,
                        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    _create_ui_command_buffers(swapChain.currentSwapChainNbImg);
    _load_fonts();
}

void
VulkanUi::clear()
{
    vkDeviceWaitIdle(_devices.device);
    ImGui_ImplVulkan_Shutdown();
    _render_pass.clear();
    vkDestroyCommandPool(_devices.device, _ui_command_pools, nullptr);
    vkDestroyDescriptorPool(_devices.device, _descriptorPool, nullptr);
    _instance = nullptr;
    _devices = VulkanDevices{};
    _queues = VulkanQueues{};
}

VkCommandBuffer
VulkanUi::generateCommandBuffer(uint32_t frameIndex,
                                VkExtent2D swapChainExtent,
                                bool noModel)
{
    VkCommandBufferBeginInfo cb_begin_info{};
    cb_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cb_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    cb_begin_info.pInheritanceInfo = nullptr;
    if (vkBeginCommandBuffer(_ui_command_buffers[frameIndex], &cb_begin_info) !=
        VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanUi: Failed to begin recording ui command buffer");
    }

    VkRenderPassBeginInfo rp_begin_info{};
    std::array<VkClearValue, 1> clear_vals{};
    if (noModel) {
        clear_vals[0].color = { { 0.2f, 0.2f, 0.2f, 1.0f } };
        rp_begin_info.renderPass = _render_pass.clearRenderPass;
    } else {
        clear_vals[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
        rp_begin_info.renderPass = _render_pass.renderPass;
    }
    rp_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin_info.framebuffer = _render_pass.framebuffers[frameIndex];
    rp_begin_info.renderArea.extent = swapChainExtent;
    rp_begin_info.clearValueCount = clear_vals.size();
    rp_begin_info.pClearValues = clear_vals.data();
    vkCmdBeginRenderPass(_ui_command_buffers[frameIndex],
                         &rp_begin_info,
                         VK_SUBPASS_CONTENTS_INLINE);
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),
                                    _ui_command_buffers[frameIndex]);
    vkCmdEndRenderPass(_ui_command_buffers[frameIndex]);
    if (vkEndCommandBuffer(_ui_command_buffers[frameIndex]) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanRenderer: Failed to record model command Buffer");
    }
    return (_ui_command_buffers[frameIndex]);
}

void
VulkanUi::_init_imgui(VulkanSwapChain const &swapChain)
{
    ImGui_ImplVulkan_InitInfo init_info = {};
    VkDescriptorPoolSize pool_sizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = static_cast<uint32_t>(IM_ARRAYSIZE(pool_sizes));
    pool_info.pPoolSizes = pool_sizes;
    if (vkCreateDescriptorPool(
          _devices.device, &pool_info, nullptr, &_descriptorPool) !=
        VK_SUCCESS) {
        throw std::runtime_error("VulkanUi: failed to create descriptor pool");
    }
    init_info.DescriptorPool = _descriptorPool;
    init_info.Instance = _instance;
    init_info.PhysicalDevice = _devices.physicalDevice;
    init_info.Device = _devices.device;
    init_info.QueueFamily = _queues.graphicFamilyIndex;
    init_info.Queue = _queues.graphicQueue;
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.Allocator = VK_NULL_HANDLE;
    init_info.MinImageCount = 2;
    init_info.ImageCount = swapChain.currentSwapChainNbImg;
    init_info.CheckVkResultFn = [](VkResult err) {
        if (err != VK_SUCCESS) {
            throw std::runtime_error("Imgui: Vulkan operation failed");
        }
    };
    ImGui_ImplVulkan_Init(&init_info, _render_pass.renderPass);
}

void
VulkanUi::_load_fonts()
{
    auto cmd_buffer =
      beginSingleTimeCommands(_devices.device, _ui_command_pools);
    ImGui_ImplVulkan_CreateFontsTexture(cmd_buffer);
    endSingleTimeCommands(
      _devices.device, _ui_command_pools, cmd_buffer, _queues.graphicQueue);
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void
VulkanUi::_create_ui_command_buffers(uint32_t nbSwapChainFrames)
{
    _ui_command_buffers.resize(nbSwapChainFrames);

    VkCommandBufferAllocateInfo cb_allocate_info{};
    cb_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cb_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cb_allocate_info.commandPool = _ui_command_pools;
    cb_allocate_info.commandBufferCount = nbSwapChainFrames;

    if (vkAllocateCommandBuffers(_devices.device,
                                 &cb_allocate_info,
                                 _ui_command_buffers.data()) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanRenderer: Failed to allocate ui command buffers");
    }
}