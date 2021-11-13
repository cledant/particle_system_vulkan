#include "common/VulkanCommonStruct.hpp"

#include "utils/VulkanMemory.hpp"

void
VulkanBuffer::allocate(VulkanDevices const &devices,
                       VkDeviceSize size,
                       VkBufferUsageFlags usage,
                       VkMemoryPropertyFlags properties)
{
    _devices = devices;
    createBuffer(devices.device, buffer, size, usage);
    allocateBuffer(
      devices.physicalDevice, devices.device, buffer, memory, properties);
}

void
VulkanBuffer::clear()
{
    vkDestroyBuffer(_devices.device, buffer, nullptr);
    vkFreeMemory(_devices.device, memory, nullptr);
    _devices = VulkanDevices{};
    buffer = nullptr;
    memory = nullptr;
}