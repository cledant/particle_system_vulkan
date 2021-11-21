#ifndef PARTICLE_SYS_VULKAN_VULKANRENDERER_HPP
#define PARTICLE_SYS_VULKAN_VULKANRENDERER_HPP

#include "glm/glm.hpp"

#include <vector>
#include <array>
#include <string>
#include <vulkan/vulkan.h>

#include "VulkanInstance.hpp"
#include "tex/VulkanTextureManager.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanSync.hpp"
#include "ui/VulkanUi.hpp"
#include "skybox/VulkanSkyboxPipeline.hpp"
#include "particle/VulkanParticlePipeline.hpp"

enum class VulkanParticleGenerationType
{
    CUBE,
    SPHERE,
    DISK,
};

class VulkanRenderer final
{
  public:
    VulkanRenderer() = default;
    ~VulkanRenderer() = default;
    VulkanRenderer(VulkanRenderer const &src) = delete;
    VulkanRenderer &operator=(VulkanRenderer const &rhs) = delete;
    VulkanRenderer(VulkanRenderer &&src) = delete;
    VulkanRenderer &operator=(VulkanRenderer &&rhs) = delete;

    // Instance related
    void createInstance(std::string &&app_name,
                        std::string &&engine_name,
                        uint32_t app_version,
                        uint32_t engine_version,
                        std::vector<char const *> &&required_extensions);
    [[nodiscard]] VkInstance getVkInstance() const;
    void init(VkSurfaceKHR surface, uint32_t win_w, uint32_t win_h);
    void resize(uint32_t win_w, uint32_t win_h);
    void clear();

    // Info related
    [[nodiscard]] std::string const &getAppName() const;
    [[nodiscard]] uint32_t getAppVersion() const;
    [[nodiscard]] std::string const &getEngineName() const;
    [[nodiscard]] uint32_t getEngineVersion() const;

    // Skybox related
    void setSkyboxInfo(glm::mat4 const &skyboxInfo);

    // Particles related
    void toggleParticlesMvt();
    void setParticleGenerationType(VulkanParticleGenerationType type);
    void setParticlesNumber(uint32_t nbParticles);
    void setParticlesColor(glm::vec3 const &particlesColor);
    void setParticleGravityCenter(glm::vec3 const &particleGravityCenter);

    // Render related
    void draw(glm::mat4 const &view_proj_mat);

    static constexpr uint64_t const DEFAULT_NB_PARTICLES = 1000000;
    static constexpr glm::vec3 const DEFAULT_PARTICLES_COLOR{ 0.0f,
                                                              0.5f,
                                                              0.3f };
    static constexpr glm::vec3 const DEFAULT_PARTICLES_GRAVITY_CENTER{};

  private:
    std::string _app_name;
    std::string _engine_name;
    uint32_t _app_version{};
    uint32_t _engine_version{};

    VulkanInstance _vk_instance;
    VulkanTextureManager _tex_manager;
    VulkanSwapChain _swap_chain;
    VulkanSync _sync;
    VulkanUi _ui;
    VulkanSkyboxPipeline _skybox;
    VulkanParticlePipeline _particle;

    // Compute shader control
    bool _doParticleGeneration = true;
    bool _doParticleMvt = false;
    bool _updateComputeCmds = true;
    VulkanParticleComputeShaderType _randomCompShader = VPCST_RANDOM_CUBE;

    // Renderer global uniform
    VulkanBuffer _system_uniform{};

    // Cmd Buffers
    std::vector<VkCommandBuffer> _render_command_buffers;
    std::vector<VkCommandBuffer> _compute_command_buffers;

    inline void recordRenderCmds();
    inline void recordComputeCmds(VulkanParticleComputeShaderType type,
                                  bool registerCmd);
    inline void selectComputeCase();
    inline void emitDrawCmds(uint32_t img_index,
                             glm::mat4 const &view_proj_mat);
};

#endif // PARTICLE_SYS_VULKAN_VULKANRENDERER_HPP
