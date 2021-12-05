#ifndef PARTICLE_SYS_VULKAN_EVENTHANDLER_HPP
#define PARTICLE_SYS_VULKAN_EVENTHANDLER_HPP

#include <chrono>

#include "IOEvents.hpp"
#include "Camera.hpp"
#include "IOManager.hpp"
#include "Perspective.hpp"
#include "VulkanRenderer.hpp"
#include "Ui.hpp"
#include "ModelInstanceInfo.hpp"

class EventHandler final
{
  public:
    EventHandler() = default;
    ~EventHandler() = default;
    EventHandler(EventHandler const &src) = delete;
    EventHandler &operator=(EventHandler const &rhs) = delete;
    EventHandler(EventHandler &&src) = delete;
    EventHandler &operator=(EventHandler &&rhs) = delete;

    void setCamera(Camera *camera);
    void setIOManager(IOManager *io_manager);
    void setPerspectiveData(Perspective *perspective);
    void setVkRenderer(VulkanRenderer *renderer);
    void setUi(Ui *ui);
    void setSkybox(ModelInstanceInfo *skybox);

    void processEvents(IOEvents const &ioEvents, UiEvent const &uiEvent);

  private:
    static constexpr double const TARGET_PLAYER_TICK = 20.0f;
    static constexpr float const SCALING_PER_SCROLL = 0.05f;

    // Timer related
    static constexpr double const SYSTEM_TIMER_SECONDS = 1.0;
    static constexpr double const CONFIG_TIMER_SECONDS = 0.5;
    static constexpr double const ACTION_TIMER_SECONDS = 0.15;
    static constexpr double const FAST_ACTION_TIMER_SECONDS = 0.01;
    static constexpr double const TARGET_PLAYER_TICK_DURATION =
      1 / TARGET_PLAYER_TICK;

    enum EventTimersTypes
    {
        ET_SYSTEM = 0,
        ET_CONFIG,
        ET_LEFT_MOUSE,
        ET_MIDDLE_MOUSE,
        ET_RIGHT_MOUSE,
        ET_CAMERA,
        ET_KEYBOARD_CONTROLS,
        ET_NB_EVENT_TIMER_TYPES
    };

    struct EventTimers final
    {
        std::array<uint8_t, ET_NB_EVENT_TIMER_TYPES> accept_event{};
        std::array<uint8_t, ET_NB_EVENT_TIMER_TYPES> updated{};
        std::array<std::chrono::steady_clock::time_point,
                   ET_NB_EVENT_TIMER_TYPES>
          time_ref{};
        std::array<double, ET_NB_EVENT_TIMER_TYPES> timer_diff{};
        std::array<double, ET_NB_EVENT_TIMER_TYPES> timer_values = {
            SYSTEM_TIMER_SECONDS,      CONFIG_TIMER_SECONDS,
            FAST_ACTION_TIMER_SECONDS, FAST_ACTION_TIMER_SECONDS,
            FAST_ACTION_TIMER_SECONDS, TARGET_PLAYER_TICK_DURATION,
            ACTION_TIMER_SECONDS
        };
    };

    // IO Event handling functions
    inline void _mouse_exclusive();
    inline void _close_win_event();
    inline void _toggle_fullscreen();
    inline void _jump();
    inline void _crouch();
    inline void _front();
    inline void _back();
    inline void _right();
    inline void _left();
    inline void _left_mouse();
    inline void _middle_mouse();
    inline void _right_mouse();
    inline void _show_fps();
    inline void _position_info();
    inline void _display_ui();
    inline void _help();
    inline void _invert_camera_y_axis();
    inline void _particle_position_update();
    inline void _reset_particles();

    // UI Event handling functions
    inline void _ui_close_app();
    inline void _ui_mouse_exclusive();
    inline void _ui_invert_mouse_y_axis();
    inline void _ui_fullscreen();
    inline void _ui_pause_start_particles();
    inline void _ui_reset_simulation();
    inline void _ui_generate_sphere();
    inline void _ui_generate_cube();
    inline void _ui_particle_number();
    inline void _ui_particle_color();
    inline void _ui_generate_disk();
    inline void _ui_particle_max_speed();

    // Camera Related
    inline void _update_camera(glm::vec2 const &mouse_pos);

    // Interaction related
    inline void _compute_mouse_3d_coordinate(glm::vec2 mouse_pos_2d);
    [[nodiscard]] inline float _compute_particle_mass() const;

    Camera *_camera{};
    IOManager *_io_manager{};
    Perspective *_perspective{};
    VulkanRenderer *_renderer{};
    Ui *_ui{};
    ModelInstanceInfo *_skybox{};

    EventTimers _timers{};

    glm::ivec3 _movements{};
    glm::vec2 _mouse_pos{};
    glm::vec2 _previous_mouse_pos{};
    bool _mouse_pos_skip = true;

    bool _invert_y_axis = false;

    // Mouse Interaction
    glm::vec2 _mouse_pos_window{};
    glm::vec3 _mouse_pos_3d{};
    glm::vec3 _gravity_center{};
    int32_t _particle_mass_multiplier{};
};

#endif // PARTICLE_SYS_VULKAN_EVENTHANDLER_HPP
