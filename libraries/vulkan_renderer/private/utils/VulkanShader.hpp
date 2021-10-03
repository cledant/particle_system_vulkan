#ifndef PARTICLE_SYS_VULKAN_VULKANSHADER_HPP
#define PARTICLE_SYS_VULKAN_VULKANSHADER_HPP

#include <vector>
#include <string>
#include <vulkan/vulkan.h>

std::vector<char> readFileContent(std::string const &filepath);
VkShaderModule createShaderModule(VkDevice device,
                                  std::vector<char> const &shaderData,
                                  std::string const &shaderName);
VkShaderModule loadShader(VkDevice device, std::string const &filepath);

#endif // PARTICLE_SYS_VULKAN_VULKANSHADER_HPP
