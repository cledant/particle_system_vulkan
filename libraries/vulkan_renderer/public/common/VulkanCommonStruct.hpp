#ifndef PARTICLE_SYSTEM_VULKAN_VULKANCOMMONSTRUCT_HPP
#define PARTICLE_SYSTEM_VULKAN_VULKANCOMMONSTRUCT_HPP

#include <string>

#include <vulkan/vulkan.h>

struct VulkanDevices final
{
    VkPhysicalDevice physicalDevice{};
    VkDevice device{};
};

struct VulkanCommandPools final
{
    VkCommandPool renderCommandPool{};
    VkCommandPool computeCommandPool{};
};

struct VulkanQueues final
{
    VkQueue graphicQueue{};
    VkQueue presentQueue{};
    VkQueue computeQueue{};
    uint32_t graphicFamilyIndex{};
    uint32_t presentFamilyIndex{};
    uint32_t computeFamilyIndex{};
};

struct VulkanBuffer final
{
    VkBuffer buffer{};
    VkDeviceMemory memory{};
    VkDeviceSize size{};

    void allocate(VulkanDevices const &devices,
                  VkDeviceSize bufferSize,
                  VkBufferUsageFlags usage,
                  VkMemoryPropertyFlags properties);
    void clear();

  private:
    VulkanDevices _devices{};
};

struct VulkanTextureStaging final
{
    VulkanBuffer stagingBuffer{};
    int32_t width{};
    int32_t height{};
    uint32_t mipLevel{};
    bool isCubemap{};

    VkDeviceSize stageTexture(VulkanDevices const &devices,
                              std::string const &filepath);
    VkDeviceSize stageTexture(VulkanDevices const &devices,
                              std::string const &cubemapFolder,
                              std::string const &filetype);
    VkDeviceSize stageTexture(VulkanDevices const &devices,
                              uint8_t const *buff,
                              int32_t texW,
                              int32_t texH,
                              int32_t nbChan,
                              bool cubemap);
    void clear();
};

struct VulkanTexture final
{
    VkImage textureImg{};
    VkDeviceMemory textureImgMemory{};
    VkImageView textureImgView{};
    VkSampler textureSampler{};
    int32_t width{};
    int32_t height{};
    uint32_t mipLevel = 1;
    VkFormat textureFormat{};
    bool isCubemap{};

    void loadTextureOnGPU(VulkanDevices const &devices,
                          VulkanCommandPools const &cmdPools,
                          VulkanQueues const &queues,
                          VulkanTextureStaging const &stagingTexture,
                          VkFormat format);
    void createDepthTexture(VulkanDevices const &devices,
                            VulkanCommandPools const &cmdPools,
                            VulkanQueues const &queues,
                            int32_t texW,
                            int32_t texH,
                            VkFormat depthFormat);
    void createSwapchainTexture(VulkanDevices const &devices,
                                VkImage swapchainImg,
                                VkFormat swapchainFormat,
                                VkExtent2D extent);
    void clear();
    void clearSwapchainTexture();

  private:
    VulkanDevices _devices;
};

#endif // PARTICLE_SYSTEM_VULKAN_VULKANCOMMONSTRUCT_HPP
