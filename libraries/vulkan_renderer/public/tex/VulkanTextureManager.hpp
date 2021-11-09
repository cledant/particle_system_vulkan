#ifndef PARTICLE_SYS_VULKAN_VULKANTEXTUREMANAGER_HPP
#define PARTICLE_SYS_VULKAN_VULKANTEXTUREMANAGER_HPP

#include <unordered_map>
#include <string>

#include <vulkan/vulkan.h>

#include "VulkanInstance.hpp"
#include "VulkanTexture.hpp"

static constexpr char const *TEX_MANAGER_DEFAULT_TEXTURE =
  "TEX_MANAGER_DEFAULT_TEXTURE";
static constexpr char const *TEX_MANAGER_DEFAULT_CUBEMAP =
  "TEX_MANAGER_DEFAULT_CUBEMAP";

class VulkanTextureManager final
{
  public:
    VulkanTextureManager() = default;
    ~VulkanTextureManager() = default;
    VulkanTextureManager(VulkanTextureManager const &src) = delete;
    VulkanTextureManager(VulkanTextureManager &&src) = delete;
    VulkanTextureManager &operator=(VulkanTextureManager const &rhs) = delete;
    VulkanTextureManager &operator=(VulkanTextureManager &&rhs) = delete;

    void init(VulkanInstance const &vkInstance);
    void clear();
    void loadTexture(std::string const &texturePath);
    void loadCubemap(std::string const &cubemapFolder,
                     std::string const &fileType);
    void unloadAllTextures();
    bool getTexture(std::string const &texturePath, VulkanTexture &tex);
    VulkanTexture loadAndGetTexture(std::string const &texturePath);
    VulkanTexture loadAndGetCubemap(std::string const &cubemapFolder,
                              std::string const &fileType);

  private:
    VulkanDevices _devices;
    VulkanQueues _queues;
    VulkanCommandPools _cmdPools;
    std::unordered_map<std::string, VulkanTexture> _textures;

    inline VkImage _create_texture_image(std::string const &texturePath,
                                         VkDeviceMemory &texture_img_memory,
                                         int32_t &tex_img_w,
                                         int32_t &tex_img_h,
                                         uint32_t &mip_level);
    inline VkImage _create_cubemap_image(std::string const &cubemapFolder,
                                         std::string const &fileType,
                                         VkDeviceMemory &texture_img_memory,
                                         int32_t &tex_img_w,
                                         int32_t &tex_img_h,
                                         uint32_t &mip_level);
    inline VkImageView _create_texture_image_view(VkImage texture_img,
                                                  uint32_t mip_level,
                                                  bool is_cubemap);
    inline VkSampler _create_texture_sampler(uint32_t mip_level);
    inline void _load_default_texture();
    inline void _load_default_cubemap();
};

#endif // PARTICLE_SYS_VULKAN_VULKANTEXTUREMANAGER_HPP
