#ifndef PARTICLE_SYS_VULKANSKYBOXPIPELINE_HPP
#define PARTICLE_SYS_VULKANSKYBOXPIPELINE_HPP

#include <vector>
#include <unordered_map>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"
#include "tex/VulkanTextureManager.hpp"
#include "VulkanSkyboxPipelineData.hpp"
#include "VulkanSkyboxPipelineDescription.hpp"
#include "renderPass/VulkanSceneRenderPass.hpp"

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
              VulkanSceneRenderPass const &renderPass,
              std::string const &skyboxFolderPath,
              std::string const &skyboxFileType,
              VulkanTextureManager &texManager,
              VkBuffer systemUbo);
    void resize(VulkanSwapChain const &swapChain,
                VulkanSceneRenderPass const &renderPass,
                VkBuffer systemUbo);
    void clear();

    void setSkyboxInfo(glm::mat4 const &skyboxInfo);

    void generateCommands(VkCommandBuffer cmdBuffer, size_t descriptorSetIndex);
    void setSkyboxModelMatOnGpu(uint32_t currentImg);

  private:
    // Vulkan related
    VulkanDevices _devices;
    VulkanCommandPools _cmdPools;
    VulkanQueues _queues;

    // Vertex / Fragment shaders related
    VulkanBuffer _skyboxUniform;
    SkyboxUbo _skyboxUbo;
    VulkanSkyboxPipelineDescription _pipelineDescription;
    VkPipeline _gfxPipeline{};

    // Global
    VulkanSkyboxPipelineData _pipelineData;
    std::vector<VkDescriptorSet> _descriptorSets;
    VkDescriptorPool _descriptorPool{};

    std::string _skyboxFolderPath;
    std::string _skyboxFiletype;

    inline void createGfxPipeline(VulkanSwapChain const &swapChain,
                                  VulkanSceneRenderPass const &renderPass);
    inline void createDescriptorSets(VulkanSkyboxPipelineData &pipelineData,
                                     VkBuffer systemUbo,
                                     uint32_t descriptorCount);
    void createDescriptorPool(uint32_t descriptorCount);
};

#endif // PARTICLE_SYS_VULKANSKYBOXPIPELINE_HPP
