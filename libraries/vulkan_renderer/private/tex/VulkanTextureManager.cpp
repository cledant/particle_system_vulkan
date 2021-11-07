#include "tex/VulkanTextureManager.hpp"

#include <stdexcept>
#include <cmath>
#include <cstring>

#include "utils/VulkanImage.hpp"
#include "utils/VulkanMemory.hpp"

void
VulkanTextureManager::init(VulkanInstance const &vkInstance)
{
    _device = vkInstance.devices.device;
    _physical_device = vkInstance.devices.physicalDevice;
    _gfx_queue = vkInstance.queues.graphicQueue;
    _command_pool = vkInstance.cmdPools.renderCommandPool;
    _load_default_texture();
    _load_default_cubemap();
}

void
VulkanTextureManager::clear()
{
    for (auto &it : _textures) {
        vkDestroySampler(_device, it.second.texture_sampler, nullptr);
        vkDestroyImageView(_device, it.second.texture_img_view, nullptr);
        vkDestroyImage(_device, it.second.texture_img, nullptr);
        vkFreeMemory(_device, it.second.texture_img_memory, nullptr);
    }
    _textures.clear();
    _device = nullptr;
    _physical_device = nullptr;
    _gfx_queue = nullptr;
    _command_pool = nullptr;
}

void
VulkanTextureManager::loadTexture(std::string const &texturePath)
{
    auto existing_tex = _textures.find(texturePath);
    if (existing_tex != _textures.end()) {
        return;
    }

    Texture tex{};

    tex.texture_img = _create_texture_image(texturePath,
                                            tex.texture_img_memory,
                                            tex.width,
                                            tex.height,
                                            tex.mip_level);
    tex.texture_img_view =
      _create_texture_image_view(tex.texture_img, tex.mip_level, false);
    tex.texture_sampler = _create_texture_sampler(tex.mip_level);
    _textures.emplace(texturePath, tex);
}

void
VulkanTextureManager::loadCubemap(std::string const &cubemapFolder,
                                  std::string const &fileType)
{
    auto existing_tex = _textures.find(cubemapFolder);
    if (existing_tex != _textures.end()) {
        return;
    }

    Texture tex{};
    tex.texture_img = _create_cubemap_image(cubemapFolder,
                                            fileType,
                                            tex.texture_img_memory,
                                            tex.width,
                                            tex.height,
                                            tex.mip_level);
    tex.texture_img_view =
      _create_texture_image_view(tex.texture_img, tex.mip_level, true);
    tex.texture_sampler = _create_texture_sampler(tex.mip_level);
    _textures.emplace(cubemapFolder, tex);
}

void
VulkanTextureManager::unloadAllTextures()
{
    for (auto &it : _textures) {
        vkDestroySampler(_device, it.second.texture_sampler, nullptr);
        vkDestroyImageView(_device, it.second.texture_img_view, nullptr);
        vkDestroyImage(_device, it.second.texture_img, nullptr);
        vkFreeMemory(_device, it.second.texture_img_memory, nullptr);
    }
    _textures.clear();
    _load_default_texture();
}

bool
VulkanTextureManager::getTexture(std::string const &texturePath, Texture &tex)
{
    auto existing_tex = _textures.find(texturePath);
    if (existing_tex != _textures.end()) {
        tex = existing_tex->second;
        return (false);
    }
    return (true);
}

Texture
VulkanTextureManager::loadAndGetTexture(std::string const &texturePath)
{
    auto existing_tex = _textures.find(texturePath);
    if (existing_tex != _textures.end()) {
        return (existing_tex->second);
    }

    Texture tex{};
    tex.texture_img = _create_texture_image(texturePath,
                                            tex.texture_img_memory,
                                            tex.width,
                                            tex.height,
                                            tex.mip_level);
    tex.texture_img_view =
      _create_texture_image_view(tex.texture_img, tex.mip_level, false);
    tex.texture_sampler = _create_texture_sampler(tex.mip_level);
    _textures.emplace(texturePath, tex);
    return (tex);
}

Texture
VulkanTextureManager::loadAndGetCubemap(std::string const &cubemapFolder,
                                        std::string const &fileType)
{
    auto existing_tex = _textures.find(cubemapFolder);
    if (existing_tex != _textures.end()) {
        return (existing_tex->second);
    }

    Texture tex{};
    tex.texture_img = _create_cubemap_image(cubemapFolder,
                                            fileType,
                                            tex.texture_img_memory,
                                            tex.width,
                                            tex.height,
                                            tex.mip_level);
    tex.texture_img_view =
      _create_texture_image_view(tex.texture_img, tex.mip_level, true);
    tex.texture_sampler = _create_texture_sampler(tex.mip_level);
    _textures.emplace(cubemapFolder, tex);
    return (tex);
}

VkImage
VulkanTextureManager::_create_texture_image(std::string const &texturePath,
                                            VkDeviceMemory &texture_img_memory,
                                            int32_t &tex_img_w,
                                            int32_t &tex_img_h,
                                            uint32_t &mip_level)
{
    VkBuffer staging_buffer{};
    VkDeviceMemory staging_buffer_memory{};

    loadTextureInBuffer(_physical_device,
                        _device,
                        texturePath,
                        staging_buffer,
                        staging_buffer_memory,
                        tex_img_w,
                        tex_img_h);
    mip_level = static_cast<uint32_t>(
                  std::floor(std::log2(std::max(tex_img_w, tex_img_h)))) +
                1;
    auto tex_img = createImage(_device,
                               tex_img_w,
                               tex_img_h,
                               mip_level,
                               VK_FORMAT_R8G8B8A8_SRGB,
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                 VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                 VK_IMAGE_USAGE_SAMPLED_BIT,
                               false);
    allocateImage(_physical_device,
                  _device,
                  tex_img,
                  texture_img_memory,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    transitionImageLayout(_device,
                          _command_pool,
                          _gfx_queue,
                          tex_img,
                          VK_FORMAT_R8G8B8A8_SRGB,
                          mip_level,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          false);
    copyBufferToImage(_device,
                      _command_pool,
                      _gfx_queue,
                      staging_buffer,
                      tex_img,
                      tex_img_w,
                      tex_img_h,
                      false);

    vkDestroyBuffer(_device, staging_buffer, nullptr);
    vkFreeMemory(_device, staging_buffer_memory, nullptr);

    generateMipmaps(_physical_device,
                    _device,
                    _command_pool,
                    _gfx_queue,
                    tex_img,
                    VK_FORMAT_R8G8B8A8_SRGB,
                    tex_img_w,
                    tex_img_h,
                    mip_level,
                    false);

    return (tex_img);
}

VkImage
VulkanTextureManager::_create_cubemap_image(std::string const &cubemapFolder,
                                            std::string const &fileType,
                                            VkDeviceMemory &texture_img_memory,
                                            int32_t &tex_img_w,
                                            int32_t &tex_img_h,
                                            uint32_t &mip_level)
{
    VkBuffer staging_buffer{};
    VkDeviceMemory staging_buffer_memory{};

    loadCubemapInBuffer(_physical_device,
                        _device,
                        cubemapFolder,
                        fileType,
                        staging_buffer,
                        staging_buffer_memory,
                        tex_img_w,
                        tex_img_h);
    mip_level = static_cast<uint32_t>(
                  std::floor(std::log2(std::max(tex_img_w, tex_img_h)))) +
                1;
    auto tex_img = createImage(_device,
                               tex_img_w,
                               tex_img_h,
                               mip_level,
                               VK_FORMAT_R8G8B8A8_SRGB,
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                 VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                 VK_IMAGE_USAGE_SAMPLED_BIT,
                               true);
    allocateImage(_physical_device,
                  _device,
                  tex_img,
                  texture_img_memory,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    transitionImageLayout(_device,
                          _command_pool,
                          _gfx_queue,
                          tex_img,
                          VK_FORMAT_R8G8B8A8_SRGB,
                          mip_level,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          true);
    copyBufferToImage(_device,
                      _command_pool,
                      _gfx_queue,
                      staging_buffer,
                      tex_img,
                      tex_img_w,
                      tex_img_h,
                      true);

    vkDestroyBuffer(_device, staging_buffer, nullptr);
    vkFreeMemory(_device, staging_buffer_memory, nullptr);

    generateMipmaps(_physical_device,
                    _device,
                    _command_pool,
                    _gfx_queue,
                    tex_img,
                    VK_FORMAT_R8G8B8A8_SRGB,
                    tex_img_w,
                    tex_img_h,
                    mip_level,
                    true);

    return (tex_img);
}

VkImageView
VulkanTextureManager::_create_texture_image_view(VkImage texture_img,
                                                 uint32_t mip_level,
                                                 bool is_cubemap)
{
    return (createImageView(texture_img,
                            VK_FORMAT_R8G8B8A8_SRGB,
                            mip_level,
                            _device,
                            VK_IMAGE_ASPECT_COLOR_BIT,
                            is_cubemap));
}

VkSampler
VulkanTextureManager::_create_texture_sampler(uint32_t mip_level)
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(_physical_device, &properties);
    float aniso = (properties.limits.maxSamplerAnisotropy > 16.0f)
                    ? 16.0f
                    : properties.limits.maxSamplerAnisotropy;

    VkSamplerCreateInfo sampler_info{};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = VK_FILTER_LINEAR;
    sampler_info.minFilter = VK_FILTER_LINEAR;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.anisotropyEnable = VK_TRUE;
    sampler_info.maxAnisotropy = aniso;
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = static_cast<float>(mip_level);

    VkSampler texture_sampler;
    if (vkCreateSampler(_device, &sampler_info, nullptr, &texture_sampler) !=
        VK_SUCCESS) {
        throw std::runtime_error(
          "VkTextureManager: failed to create texture sampler");
    }
    return (texture_sampler);
}

void
VulkanTextureManager::_load_default_texture()
{
    static uint8_t const white_tex[4] = { 255, 255, 255, 255 };
    Texture tex{
        nullptr, nullptr, nullptr, nullptr, 1, 1, 1,
    };

    // Staging buffer
    VkBuffer staging_buffer{};
    VkDeviceMemory staging_buffer_memory{};
    createBuffer(_device, staging_buffer, 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    allocateBuffer(_physical_device,
                   _device,
                   staging_buffer,
                   staging_buffer_memory,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    void *data;
    vkMapMemory(_device, staging_buffer_memory, 0, 4, 0, &data);
    memcpy(data, white_tex, 4);
    vkUnmapMemory(_device, staging_buffer_memory);

    // Texture
    tex.texture_img = createImage(_device,
                                  tex.width,
                                  tex.height,
                                  tex.mip_level,
                                  VK_FORMAT_R8G8B8A8_SRGB,
                                  VK_IMAGE_TILING_OPTIMAL,
                                  VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                    VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                    VK_IMAGE_USAGE_SAMPLED_BIT,
                                  false);
    allocateImage(_physical_device,
                  _device,
                  tex.texture_img,
                  tex.texture_img_memory,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    transitionImageLayout(_device,
                          _command_pool,
                          _gfx_queue,
                          tex.texture_img,
                          VK_FORMAT_R8G8B8A8_SRGB,
                          1,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          false);
    copyBufferToImage(_device,
                      _command_pool,
                      _gfx_queue,
                      staging_buffer,
                      tex.texture_img,
                      1,
                      1,
                      false);

    vkDestroyBuffer(_device, staging_buffer, nullptr);
    vkFreeMemory(_device, staging_buffer_memory, nullptr);

    tex.texture_img_view =
      _create_texture_image_view(tex.texture_img, tex.mip_level, false);
    tex.texture_sampler = _create_texture_sampler(tex.mip_level);

    generateMipmaps(_physical_device,
                    _device,
                    _command_pool,
                    _gfx_queue,
                    tex.texture_img,
                    VK_FORMAT_R8G8B8A8_SRGB,
                    tex.width,
                    tex.height,
                    tex.mip_level,
                    false);

    _textures.emplace(TEX_MANAGER_DEFAULT_TEXTURE, tex);
}

void
VulkanTextureManager::_load_default_cubemap()
{
    static constexpr uint8_t const white_tex_size = 4 * 6;
    static uint8_t const white_tex[white_tex_size] = {
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
    };
    Texture tex{
        nullptr, nullptr, nullptr, nullptr, 1, 1, 1,
    };

    // Staging buffer
    VkBuffer staging_buffer{};
    VkDeviceMemory staging_buffer_memory{};
    createBuffer(_device,
                 staging_buffer,
                 white_tex_size,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    allocateBuffer(_physical_device,
                   _device,
                   staging_buffer,
                   staging_buffer_memory,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    void *data;
    vkMapMemory(_device, staging_buffer_memory, 0, white_tex_size, 0, &data);
    memcpy(data, white_tex, white_tex_size);
    vkUnmapMemory(_device, staging_buffer_memory);

    // Texture
    tex.texture_img = createImage(_device,
                                  tex.width,
                                  tex.height,
                                  tex.mip_level,
                                  VK_FORMAT_R8G8B8A8_SRGB,
                                  VK_IMAGE_TILING_OPTIMAL,
                                  VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                    VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                    VK_IMAGE_USAGE_SAMPLED_BIT,
                                  true);
    allocateImage(_physical_device,
                  _device,
                  tex.texture_img,
                  tex.texture_img_memory,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    transitionImageLayout(_device,
                          _command_pool,
                          _gfx_queue,
                          tex.texture_img,
                          VK_FORMAT_R8G8B8A8_SRGB,
                          1,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          true);
    copyBufferToImage(_device,
                      _command_pool,
                      _gfx_queue,
                      staging_buffer,
                      tex.texture_img,
                      1,
                      1,
                      true);

    vkDestroyBuffer(_device, staging_buffer, nullptr);
    vkFreeMemory(_device, staging_buffer_memory, nullptr);

    tex.texture_img_view =
      _create_texture_image_view(tex.texture_img, tex.mip_level, true);
    tex.texture_sampler = _create_texture_sampler(tex.mip_level);

    generateMipmaps(_physical_device,
                    _device,
                    _command_pool,
                    _gfx_queue,
                    tex.texture_img,
                    VK_FORMAT_R8G8B8A8_SRGB,
                    tex.width,
                    tex.height,
                    tex.mip_level,
                    true);

    _textures.emplace(TEX_MANAGER_DEFAULT_CUBEMAP, tex);
}
