#include "tex/VulkanTextureManager.hpp"

#include "utils/VulkanMemory.hpp"
#include "utils/VulkanTextureUtils.hpp"

void
VulkanTextureManager::init(VulkanInstance const &vkInstance)
{
    _devices = vkInstance.devices;
    _queues = vkInstance.queues;
    _cmdPools = vkInstance.cmdPools;
    _load_default_texture();
    _load_default_cubemap();
}

void
VulkanTextureManager::clear()
{
    for (auto &it : _textures) {
        vkDestroySampler(_devices.device, it.second.textureSampler, nullptr);
        vkDestroyImageView(_devices.device, it.second.textureImgView, nullptr);
        vkDestroyImage(_devices.device, it.second.textureImg, nullptr);
        vkFreeMemory(_devices.device, it.second.textureImgMemory, nullptr);
    }
    _textures.clear();
    _devices.device = nullptr;
    _devices.physicalDevice = nullptr;
    _queues.graphicQueue = nullptr;
    _cmdPools.renderCommandPool = nullptr;
}

void
VulkanTextureManager::loadTexture(std::string const &texturePath)
{
    auto existing_tex = _textures.find(texturePath);
    if (existing_tex != _textures.end()) {
        return;
    }

    VulkanTextureStaging stagingTex{};
    stagingTex.stageTexture(_devices, texturePath);

    VulkanTexture gpuTexture{};
    gpuTexture.loadTextureOnGPU(
      _devices, _cmdPools, _queues, stagingTex, VK_FORMAT_R8G8B8A8_SRGB);
    stagingTex.clear();
    _textures.emplace(texturePath, gpuTexture);
}

void
VulkanTextureManager::loadCubemap(std::string const &cubemapFolder,
                                  std::string const &fileType)
{
    auto existing_tex = _textures.find(cubemapFolder);
    if (existing_tex != _textures.end()) {
        return;
    }

    VulkanTextureStaging stagingTex{};
    stagingTex.stageTexture(_devices, cubemapFolder, fileType);

    VulkanTexture gpuTexture{};
    gpuTexture.loadTextureOnGPU(
      _devices, _cmdPools, _queues, stagingTex, VK_FORMAT_R8G8B8A8_SRGB);
    stagingTex.clear();
    _textures.emplace(cubemapFolder, gpuTexture);
}

void
VulkanTextureManager::addTexture(std::string const &textureName,
                                 VulkanTexture &&texture)
{
    _textures.emplace(textureName, texture);
}

void
VulkanTextureManager::unloadAllTextures()
{
    for (auto &it : _textures) {
        it.second.clear();
    }
    _textures.clear();
    _load_default_texture();
}

bool
VulkanTextureManager::getTexture(std::string const &texturePath,
                                 VulkanTexture &tex)
{
    auto existing_tex = _textures.find(texturePath);
    if (existing_tex != _textures.end()) {
        tex = existing_tex->second;
        return (false);
    }
    return (true);
}

VulkanTexture
VulkanTextureManager::loadAndGetTexture(std::string const &texturePath)
{
    auto existing_tex = _textures.find(texturePath);
    if (existing_tex != _textures.end()) {
        return (existing_tex->second);
    }

    VulkanTextureStaging stagingTex{};
    stagingTex.stageTexture(_devices, texturePath);

    VulkanTexture gpuTexture{};
    gpuTexture.loadTextureOnGPU(
      _devices, _cmdPools, _queues, stagingTex, VK_FORMAT_R8G8B8A8_SRGB);
    stagingTex.clear();
    _textures.emplace(texturePath, gpuTexture);
    return (gpuTexture);
}

VulkanTexture
VulkanTextureManager::loadAndGetCubemap(std::string const &cubemapFolder,
                                        std::string const &fileType)
{
    auto existing_tex = _textures.find(cubemapFolder);
    if (existing_tex != _textures.end()) {
        return (existing_tex->second);
    }

    VulkanTextureStaging stagingTex{};
    stagingTex.stageTexture(_devices, cubemapFolder, fileType);

    VulkanTexture gpuTexture{};
    gpuTexture.loadTextureOnGPU(
      _devices, _cmdPools, _queues, stagingTex, VK_FORMAT_R8G8B8A8_SRGB);
    stagingTex.clear();
    _textures.emplace(cubemapFolder, gpuTexture);
    return (gpuTexture);
}

void
VulkanTextureManager::_load_default_texture()
{
    static uint8_t const white_tex[4] = { 255, 255, 255, 255 };

    VulkanTextureStaging stagingTex{};
    stagingTex.stageTexture(_devices, white_tex, 1, 1, 4, false);

    VulkanTexture gpuTexture{};
    gpuTexture.loadTextureOnGPU(
      _devices, _cmdPools, _queues, stagingTex, VK_FORMAT_R8G8B8A8_SRGB);
    stagingTex.clear();
    _textures.emplace(TEX_MANAGER_DEFAULT_TEXTURE, gpuTexture);
}

void
VulkanTextureManager::_load_default_cubemap()
{
    static uint8_t const white_tex[] = {
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
    };

    VulkanTextureStaging stagingTex{};
    stagingTex.stageTexture(_devices, white_tex, 1, 1, 4, true);

    VulkanTexture gpuTexture{};
    gpuTexture.loadTextureOnGPU(
      _devices, _cmdPools, _queues, stagingTex, VK_FORMAT_R8G8B8A8_SRGB);
    stagingTex.clear();
    _textures.emplace(TEX_MANAGER_DEFAULT_CUBEMAP, gpuTexture);
}
