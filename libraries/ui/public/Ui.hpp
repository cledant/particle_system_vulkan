#ifndef PARTICLE_SYS_VULKAN_UI_HPP
#define PARTICLE_SYS_VULKAN_UI_HPP

#include <chrono>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"
#include "UiInfoOverview.hpp"
#include "UiSimpleInputWindow.hpp"
#include "UiRGBColorInputWindow.hpp"
#include "UiAboutBox.hpp"
#include "UiTextBox.hpp"

enum UiEventTypes
{
    UET_EXIT,
    UET_MOUSE_EXCLUSIVE,
    UET_INVERT_MOUSE_AXIS,
    UET_FULLSCREEN,
    UET_PAUSE_START_PARTICLES,
    UET_RESET_PARTICLES,
    UET_GENERATE_SPHERE,
    UET_GENERATE_CUBE,
    UET_SET_PARTICLES_NUMBER,
    UET_SET_PARTICLES_COLOR,
    UET_GENERATE_DISK,
    UET_SET_PARTICLE_MAX_SPEED,
    UET_TOTAL_NB,
};

struct UiEvent
{
    bool events[UET_TOTAL_NB] = { false };
};

class Ui final
{
  public:
    Ui() = default;
    ~Ui() = default;
    Ui(Ui const &src) = delete;
    Ui &operator=(Ui const &rhs) = delete;
    Ui(Ui &&src) = delete;
    Ui &operator=(Ui &&rhs) = delete;

    void init(GLFWwindow *win);
    void clear();

    [[nodiscard]] UiEvent getUiEvent() const;
    [[nodiscard]] uint32_t getNbParticles() const;
    [[nodiscard]] uint32_t getParticleMaxSpeed() const;
    [[nodiscard]] glm::vec3 getParticlesColor() const;
    [[nodiscard]] bool isUiHovered() const;

    // Trigger from keyboard
    void toggleInfoPosition();
    void toggleShowFps();
    void toggleHelp();
    void toggleAbout();
    void toggleDisplayUi();
    void toggleFullscreen();
    void toggleCameraMvt();
    void toggleInvertCameraYAxis();

    // Info
    void setCameraPos(glm::vec3 const &cameraPos);
    void setCursorPositionWindow(glm::vec2 const &cursorPos);
    void setCursorPosition3D(glm::vec3 const &cursorPos);
    void setGravityCenterPos(glm::vec3 const &gravityCenterPos);
    void setNbParticles(uint32_t nbParticles);
    void setParticleMaxSpeed(uint32_t maxSpeed);

    void drawUi();

  private:
    // File

    // Edit
    bool _generate_sphere = false;
    bool _generate_cube = true;
    bool _generate_disk = false;
    uint32_t _nb_particles{};
    uint32_t _max_speed_particles{};
    UiSimpleInputWindow _particle_input_win{};
    UiSimpleInputWindow _max_speed_particles_input_win{};
    UiRGBColorInputWindow _particle_color_input;
    void _draw_edit_panel();
    void _draw_particles_nb_input();
    void _draw_particles_speed_input();

    // Controls
    bool _toggle_camera_mvt = false;
    bool _invert_camera_y_axis = false;

    // View
    bool _fullscreen = false;
    bool _display_ui = true;
    bool _show_info_position = false;
    bool _show_info_fps = false;

    // Help
    UiAboutBox _about_box;
    UiTextBox _help_box;

    // Menu Bar
    void _draw_menu_bar();

    // Compute fps
    void _compute_fps();
    uint64_t _nb_frame{};
    std::chrono::steady_clock::time_point _avg_fps_time_ref;
    std::chrono::steady_clock::time_point _prev_frame_time_ref;

    // Events from ui interaction
    UiEvent _ui_events{};

    // Informations
    UiInfoOverview _info_overview;
};

#endif // PARTICLE_SYS_VULKAN_UI_HPP
