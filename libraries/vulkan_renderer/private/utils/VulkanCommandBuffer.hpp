#ifndef PARTICLE_SYS_VULKAN_VULKANCOMMANDBUFFER_HPP
#define PARTICLE_SYS_VULKAN_VULKANCOMMANDBUFFER_HPP

#include <vector>

#include <vulkan/vulkan.h>

VkCommandBuffer beginSingleTimeCommands(VkDevice device,
                                        VkCommandPool command_pool);
void endSingleTimeCommands(VkDevice device,
                           VkCommandPool command_pool,
                           VkCommandBuffer command_buffer,
                           VkQueue gfx_queue);
VkCommandPool createCommandPool(VkDevice device,
                                uint32_t queueIndex,
                                VkCommandPoolCreateFlags flags);
void allocateCommandBuffers(VkDevice device,
                            VkCommandPool cmdPool,
                            std::vector<VkCommandBuffer> &cmdBuffers,
                            uint32_t nbCmdBuffers);

#endif // PARTICLE_SYS_VULKAN_VULKANCOMMANDBUFFER_HPP
