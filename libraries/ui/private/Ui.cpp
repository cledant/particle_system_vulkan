#include "Ui.hpp"

#include <chrono>

#include "fmt/format.h"

void
Ui::init(GLFWwindow *win)
{
    assert(win);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(win, true);
    _avg_fps_time_ref = std::chrono::steady_clock::now();
    // Init input nb particle
    _particle_input_win.windowName = "Set number of particles";
    _particle_input_win.windowText = "Particles";
    _particle_input_win.winW = 300;
    // Init input max speed particle
    _max_speed_particles_input_win.windowName = "Set particles max speed";
    _max_speed_particles_input_win.windowText = "m/s";
    _max_speed_particles_input_win.winW = 300;
    // Init input particle color
    _particle_color_input.windowName = "Particles color selection";
}

void
Ui::clear()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

UiEvent
Ui::getUiEvent() const
{
    return (_ui_events);
}

uint32_t
Ui::getNbParticles() const
{
    return (_nb_particles);
}

uint32_t
Ui::getParticleMaxSpeed() const
{
    return (_max_speed_particles);
}

glm::vec3
Ui::getParticlesColor() const
{
    return (glm::vec3(_particle_color_input.color.x,
                      _particle_color_input.color.y,
                      _particle_color_input.color.z));
}

bool
Ui::isUiHovered() const
{
    return (ImGui::IsAnyItemHovered() ||
            ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow));
}

void
Ui::toggleInfoPosition()
{
    _show_info_position = !_show_info_position;
}

void
Ui::toggleShowFps()
{
    _show_info_fps = !_show_info_fps;
}

void
Ui::toggleAbout()
{
    _about_box.isOpen = !_about_box.isOpen;
}

void
Ui::toggleDisplayUi()
{
    _display_ui = !_display_ui;
}

void
Ui::toggleFullscreen()
{
    _fullscreen = !_fullscreen;
}

void
Ui::toggleCameraMvt()
{
    _toggle_camera_mvt = !_toggle_camera_mvt;
}

void
Ui::toggleInvertCameraYAxis()
{
    _invert_camera_y_axis = !_invert_camera_y_axis;
}

// Position Info
void
Ui::setCameraPos(glm::vec3 const &cameraPos)
{
    _info_overview.cameraPos = cameraPos;
}

void
Ui::setCursorPositionWindow(glm::vec2 const &cursorPos)
{
    _info_overview.cursorPositionWindow = cursorPos;
}

void
Ui::setCursorPosition3D(glm::vec3 const &cursorPos)
{
    _info_overview.cursorPosition3D = cursorPos;
}

void
Ui::setGravityCenterPos(glm::vec3 const &gravityCenterPos)
{
    _info_overview.gravityCenterPos = gravityCenterPos;
}

void
Ui::setNbParticles(uint32_t nbParticles)
{
    _info_overview.nbParticles = nbParticles;
    _nb_particles = nbParticles;
}

void
Ui::setParticleMaxSpeed(uint32_t maxSpeed)
{
    _max_speed_particles = maxSpeed;
    _info_overview.maxSpeedParticle = maxSpeed;
}

void
Ui::drawUi()
{
    _compute_fps();
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (!_display_ui) {
        ImGui::Render();
        return;
    }

    _draw_menu_bar();
    _info_overview.draw(_show_info_fps, _show_info_position);
    _about_box.draw();
    _ui_events.events[UET_SET_PARTICLES_COLOR] =
      _particle_color_input.drawInputWindow();

    auto trigger_nb_particle = _particle_input_win.drawInputWindow();
    if (trigger_nb_particle) {
        try {
            auto parsed_nb_particle = std::stoul(_particle_input_win.input);
            if (parsed_nb_particle <= UINT32_MAX) {
                _ui_events.events[UET_SET_PARTICLES_NUMBER] = true;
                _nb_particles = parsed_nb_particle;
                _info_overview.nbParticles = _nb_particles;
            } else {
                _particle_input_win.isInputOpen = false;
                _particle_input_win.isErrorOpen = true;
                _particle_input_win.errorText = fmt::format(
                  "Number should be between {} and {}", 0, UINT32_MAX);
            }
        } catch (std::exception const &e) {
            _particle_input_win.isInputOpen = false;
            _particle_input_win.isErrorOpen = true;
            _particle_input_win.errorText = "Invalid number";
        }
    }
    _particle_input_win.drawInputErrorWindow();

    auto trigger_max_speed_particle =
      _max_speed_particles_input_win.drawInputWindow();
    if (trigger_max_speed_particle) {
        try {
            auto parsed_max_speed =
              std::stoul(_max_speed_particles_input_win.input);
            if (parsed_max_speed <= UINT32_MAX) {
                _max_speed_particles = parsed_max_speed;
                _ui_events.events[UET_SET_PARTICLE_MAX_SPEED] = true;
                _info_overview.maxSpeedParticle = _max_speed_particles;
            } else {
                _max_speed_particles_input_win.isInputOpen = false;
                _max_speed_particles_input_win.isErrorOpen = true;
                _max_speed_particles_input_win.errorText = fmt::format(
                  "Number should be between {} and {}", 0, UINT32_MAX);
            }
        } catch (std::exception const &e) {
            _max_speed_particles_input_win.isInputOpen = false;
            _max_speed_particles_input_win.isErrorOpen = true;
            _max_speed_particles_input_win.errorText = "Invalid number";
        }
    }
    _max_speed_particles_input_win.drawInputErrorWindow();
    ImGui::Render();
}

void
Ui::_draw_edit_panel()
{
    if (ImGui::MenuItem("Set Number of particles")) {
        _particle_input_win.isInputOpen = !_particle_input_win.isInputOpen;
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Set Particles color")) {
        _particle_color_input.isInputOpen = !_particle_color_input.isInputOpen;
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Set Particles max speed")) {
        _max_speed_particles_input_win.isInputOpen =
          !_max_speed_particles_input_win.isInputOpen;
    }
    ImGui::Separator();
    _ui_events.events[UET_PAUSE_START_PARTICLES] =
      ImGui::MenuItem("Pause / Resume particles movement", "F2");
    ImGui::Separator();
    _ui_events.events[UET_RESET_PARTICLES] =
      ImGui::MenuItem("Reset simulation", "F3");
    ImGui::Separator();
    if (ImGui::BeginMenu("Particle distribution")) {
        if ((_ui_events.events[UET_GENERATE_SPHERE] =
               ImGui::MenuItem("Sphere", "", &_generate_sphere))) {
            _generate_cube = false;
            _generate_disk = false;
        }
        if ((_ui_events.events[UET_GENERATE_CUBE] =
               ImGui::MenuItem("Cube", "", &_generate_cube))) {
            _generate_sphere = false;
            _generate_disk = false;
        }
        if ((_ui_events.events[UET_GENERATE_DISK] =
               ImGui::MenuItem("Disk", "", &_generate_disk))) {
            _generate_sphere = false;
            _generate_cube = false;
        }
        ImGui::EndMenu();
    }
    ImGui::EndMenu();
}

void
Ui::_draw_menu_bar()
{
    _ui_events = {};
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            _ui_events.events[UET_EXIT] = ImGui::MenuItem("Exit", "F10");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            _draw_edit_panel();
        }
        if (ImGui::BeginMenu("Controls")) {
            _ui_events.events[UET_MOUSE_EXCLUSIVE] = ImGui::MenuItem(
              "Toggle Camera Movement", "F4", &_toggle_camera_mvt);
            ImGui::Separator();
            _ui_events.events[UET_INVERT_MOUSE_AXIS] = ImGui::MenuItem(
              "Inverse Mouse Y Axis", "F5", &_invert_camera_y_axis);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Info", "F6", &_show_info_position);
            ImGui::Separator();
            ImGui::MenuItem("Show Fps", "F7", &_show_info_fps);
            ImGui::Separator();
            _ui_events.events[UET_FULLSCREEN] =
              ImGui::MenuItem("Fullscreen", "F8", &_fullscreen);
            ImGui::Separator();
            ImGui::MenuItem("Display UI", "F9", &_display_ui);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About", "F1")) {
                _about_box.isOpen = !_about_box.isOpen;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void
Ui::_compute_fps()
{
    ++_nb_frame;
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff_frame = now - _prev_frame_time_ref;
    _info_overview.currentFps = 1.0f / diff_frame.count();
    _prev_frame_time_ref = now;

    std::chrono::duration<double> diff_avg = now - _avg_fps_time_ref;
    if (diff_avg.count() > 1.0f) {
        _info_overview.avgFps = _nb_frame;
        _nb_frame = 0;
        _avg_fps_time_ref = now;
    }
}
