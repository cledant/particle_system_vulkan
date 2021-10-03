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

    [[nodiscard]] std::string const &getAppName() const;
    [[nodiscard]] uint32_t getAppVersion() const;
    [[nodiscard]] std::string const &getEngineName() const;
    [[nodiscard]] uint32_t getEngineVersion() const;

    // Render related
    void draw(glm::mat4 const &view_proj_mat);
    void deviceWaitIdle() const;

  private:
    static constexpr uint32_t MAX_MODEL_INSTANCE = 10;

    std::string _app_name;
    std::string _engine_name;
    uint32_t _app_version{};
    uint32_t _engine_version{};

    VulkanInstance _vk_instance;
    VulkanTextureManager _tex_manager;
    VulkanSwapChain _swap_chain;
    VulkanSync _sync;
    VulkanUi _ui;

    // Renderer global uniform
    VkBuffer _system_uniform{};
    VkDeviceMemory _system_uniform_memory{};

    // Drawing related
    std::vector<VkCommandBuffer> _model_command_buffers;

    // Draw related fct
    inline void _create_model_command_buffers();

    // Renderer global uniform related fct
    inline void _create_system_uniform_buffer();

    // Draw command emission related
    inline void _emit_model_ui_cmds(uint32_t img_index,
                                    glm::mat4 const &view_proj_mat);
    inline void _emit_ui_cmds(uint32_t img_index);
};

#endif // PARTICLE_SYS_VULKAN_VULKANRENDERER_HPP
