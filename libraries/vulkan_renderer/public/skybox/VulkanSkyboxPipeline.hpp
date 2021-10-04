#ifndef PARTICLE_SYS_VULKANSKYBOXPIPELINE_HPP
#define PARTICLE_SYS_VULKANSKYBOXPIPELINE_HPP

#include <vector>
#include <unordered_map>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanSkyboxRenderPass.hpp"
#include "tex/VulkanTextureManager.hpp"
#include "VulkanSkyboxPipelineData.hpp"
#include "VulkanSkyboxRenderPass.hpp"

class VulkanSkyboxPipeline final
{
  public:
    VulkanSkyboxPipeline() = default;
    ~VulkanSkyboxPipeline() = default;
    VulkanSkyboxPipeline(VulkanSkyboxPipeline const &src) = delete;
    VulkanSkyboxPipeline &operator=(VulkanSkyboxPipeline const &rhs) = delete;
    VulkanSkyboxPipeline(VulkanSkyboxPipeline &&src) = delete;
    VulkanSkyboxPipeline &operator=(VulkanSkyboxPipeline &&rhs) = delete;

    void init(VulkanInstance const &vkInstance,
              VulkanSwapChain const &swapChain,
              std::string const &skyboxFolderPath,
              VulkanTextureManager &texManager,
              VkBuffer systemUbo);
    void resize(VulkanSwapChain const &swapChain,
                VulkanTextureManager &texManager,
                VkBuffer systemUbo);
    void clear();

    void setSkyboxTexture(std::string const &skyboxFolderPath,
                          VulkanTextureManager &texManager);
    void setSkyboxInfo(glm::mat4 const &skyboxInfo);
    VulkanSkyboxRenderPass const &getVulkanSkyboxRenderPass() const;

    void generateCommands(VkCommandBuffer cmdBuffer,
                          size_t descriptorSetIndex,
                          uint32_t currentSwapChainNbImg);

  private:
    // Vulkan related
    VkDevice _device{};
    VkPhysicalDevice _physical_device{};
    VkCommandPool _cmd_pool{};
    VkQueue _gfx_queue{};
    VkDescriptorSetLayout _descriptor_set_layout{};
    VkPipelineLayout _pipeline_layout{};
    VkPipeline _graphic_pipeline{};
    VulkanSkyboxPipelineData _pipeline_data;
    VulkanSkyboxRenderPass _pipeline_render_pass;

    Texture _current_tex{};
    Texture _default_tex{};

    inline void _create_descriptor_layout();
    inline void _create_pipeline_layout();
    inline void _create_gfx_pipeline(VulkanSwapChain const &swapChain);
    inline VulkanSkyboxPipelineData _create_pipeline_skybox(
      std::string const &modelFolder,
      VulkanTextureManager &textureManager,
      uint32_t currentSwapChainNbImg);
    inline void _create_descriptor_pool(VulkanSwapChain const &swapChain,
                                        VulkanSkyboxPipelineData &pipelineData);
    inline void _create_descriptor_sets(VulkanSwapChain const &swapChain,
                                        VulkanSkyboxPipelineData &pipelineData,
                                        VkBuffer systemUbo);
};

#endif // PARTICLE_SYS_VULKANSKYBOXPIPELINE_HPP
