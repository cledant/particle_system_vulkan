#ifndef PARTICLE_SYS_VULKAN_VULKANTEXTUREMANAGER_HPP
#define PARTICLE_SYS_VULKAN_VULKANTEXTUREMANAGER_HPP

#include <unordered_map>
#include <string>

#include <vulkan/vulkan.h>

#include "VulkanInstance.hpp"

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
    void addTexture(std::string const &textureName, VulkanTexture &&texture);
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

    inline void _load_default_texture();
    inline void _load_default_cubemap();
};

#endif // PARTICLE_SYS_VULKAN_VULKANTEXTUREMANAGER_HPP
