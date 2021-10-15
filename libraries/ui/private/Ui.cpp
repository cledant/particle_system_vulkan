#include "Ui.hpp"

#include <chrono>

#include "AppVersion.hpp"

void
Ui::init(GLFWwindow *win)
{
    assert(win);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(win, true);
    _avg_fps_time_ref = std::chrono::steady_clock::now();
    _particle_input_win.windowName = "Set number of particles";
    _particle_input_win.windowText = "Particles";
    _particle_input_win.winW = 300;
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

uint64_t
Ui::getNbParticles() const
{
    return (_nb_particles);
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
    _about = !_about;
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
Ui::setNbParticles(uint64_t nbParticles)
{
    _info_overview.nbParticles = nbParticles;
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
    _draw_about_info_box();
    _info_overview.draw(_show_info_fps, _show_info_position);
    auto trigger_nb_particle = _particle_input_win.drawInputWindow();
    if (trigger_nb_particle) {
        try {
            _nb_particles = std::stoi(_particle_input_win.input);
            _ui_events.events[UET_SET_PARTICLE_NUMBER] = true;
            _info_overview.nbParticles = _nb_particles;
        } catch (std::exception const &e) {
            _particle_input_win.isInputOpen = false;
            _particle_input_win.isErrorOpen = true;
            _particle_input_win.errorText = "Invalid number";
        }
    }
    _particle_input_win.drawInputErrorWindow();
    ImGui::Render();
}

void
Ui::_draw_edit_panel()
{
    if (ImGui::MenuItem("Set Number of particles")) {
        _particle_input_win.isInputOpen = !_particle_input_win.isInputOpen;
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
        }
        if ((_ui_events.events[UET_GENERATE_CUBE] =
               ImGui::MenuItem("Cube", "", &_generate_cube))) {
            _generate_sphere = false;
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
                _about = !_about;
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

void
Ui::_draw_about_info_box()
{
    static constexpr ImGuiWindowFlags const WIN_FLAGS =
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoMove;
    static ImVec2 const WIN_POS_PIVOT = { 0.5f, 0.5f };

    if (_about) {
        ImGuiViewport const *viewport = ImGui::GetMainViewport();
        auto viewport_center = viewport->GetCenter();
        ImVec2 window_pos{ viewport_center.x, viewport_center.y };

        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, WIN_POS_PIVOT);
        ImGui::Begin("About", &_about, WIN_FLAGS);
        ImGui::Text("particle_system");
        ImGui::Separator();
        ImGui::Text("Version: %d.%d.%d",
                    particle_sys::APP_VERSION_MAJOR,
                    particle_sys::APP_VERSION_MINOR,
                    particle_sys::APP_VERSION_PATCH);
        ImGui::Separator();
        ImGui::Text("Commit: %s", particle_sys::APP_COMMIT_HASH);
        ImGui::End();
    }
}
