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
              std::string const &skyboxFileType,
              VulkanTextureManager &texManager,
              VkBuffer systemUbo);
    void resize(VulkanSwapChain const &swapChain,
                VulkanTextureManager &texManager,
                VkBuffer systemUbo);
    void clear();

    void setSkyboxInfo(glm::mat4 const &skyboxInfo);

    [[nodiscard]] VulkanSkyboxRenderPass const &getVulkanSkyboxRenderPass()
      const;
    void generateCommands(VkCommandBuffer cmdBuffer, size_t descriptorSetIndex);
    void setSkyboxModelMatOnGpu(uint32_t currentImg);

  private:
    // Vulkan related
    VulkanDevices _devices;
    VulkanCommandPools _cmdPools;
    VulkanQueues _queues;

    VkDescriptorSetLayout _descriptor_set_layout{};
    VkPipelineLayout _pipeline_layout{};
    VkPipeline _graphic_pipeline{};
    VulkanSkyboxPipelineData _pipeline_data;
    VulkanSkyboxRenderPass _pipeline_render_pass;

    // Skybox related
    VkBuffer _skybox_uniform{};
    VkDeviceMemory _skybox_uniform_memory{};
    Texture _skybox_tex{};
    std::string _skybox_folder_path;
    std::string _skybox_filetype;
    glm::mat4 _skybox_model{};

    inline void _create_descriptor_layout();
    inline void _create_pipeline_layout();
    inline void _create_gfx_pipeline(VulkanSwapChain const &swapChain);
    inline VulkanSkyboxPipelineData _create_pipeline_skybox();
    inline void _create_descriptor_pool(VulkanSwapChain const &swapChain,
                                        VulkanSkyboxPipelineData &pipelineData);
    inline void _create_descriptor_sets(VulkanSwapChain const &swapChain,
                                        VulkanSkyboxPipelineData &pipelineData,
                                        VkBuffer systemUbo);
    inline void _create_skybox_uniform_buffer(uint32_t currentSwapChainNbImg);
};

#endif // PARTICLE_SYS_VULKANSKYBOXPIPELINE_HPP
