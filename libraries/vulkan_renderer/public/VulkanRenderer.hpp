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
#include "particleDisplayDebug/VulkanParticleDebugPipeline.hpp"

enum class VulkanParticleGenerationType
{
    CUBE,
    SPHERE,
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
    void toggleUpdateParticlesPosition();
    void setParticleGenerationType(VulkanParticleGenerationType type);
    void setParticlesNumber(uint64_t nbParticles);
    void setParticlesColor(glm::vec3 const &particlesColor);
    void setParticleGravityCenter(glm::vec3 const &particleGravityCenter);
    static constexpr uint64_t const DEFAULT_NB_PARTICLES = 1000000;
    static constexpr glm::vec3 const DEFAULT_PARTICLES_COLOR{ 0.0f,
                                                              0.5f,
                                                              0.3f };
    static constexpr glm::vec3 const DEFAULT_PARTICLES_GRAVITY_CENTER{};

    // Render related
    void draw(glm::mat4 const &view_proj_mat);
    void deviceWaitIdle() const;

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
    VulkanParticleDebugPipeline _particle;
    bool _update_particle_positions{};
    VulkanParticleGenerationType _particle_generation_type{};

    // Renderer global uniform
    VkBuffer _system_uniform{};
    VkDeviceMemory _system_uniform_memory{};

    // Drawing related
    std::vector<VkCommandBuffer> _render_command_buffers;

    // Draw related fct
    inline void _create_render_command_buffers();

    // Renderer global uniform related fct
    inline void _create_system_uniform_buffer();

    // Draw command emission related
    inline void _emit_render_and_ui_cmds(uint32_t img_index,
                                         glm::mat4 const &view_proj_mat);
};

#endif // PARTICLE_SYS_VULKAN_VULKANRENDERER_HPP
