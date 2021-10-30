#include "EventHandler.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/gtc/matrix_transform.hpp"

#include <functional>

void
EventHandler::setCamera(Camera *camera)
{
    _camera = camera;
}

void
EventHandler::setIOManager(IOManager *io_manager)
{
    _io_manager = io_manager;
}

void
EventHandler::setPerspectiveData(Perspective *perspective)
{
    _perspective = perspective;
}

void
EventHandler::setVkRenderer(VulkanRenderer *renderer)
{
    _renderer = renderer;
}

void
EventHandler::setUi(Ui *ui)
{
    _ui = ui;
}

void
EventHandler::setSkybox(ModelInstanceInfo *skybox)
{
    _skybox = skybox;
}

void
EventHandler::processEvents(IOEvents const &ioEvents, UiEvent const &uiEvent)
{
    assert(_camera);
    assert(_io_manager);
    assert(_perspective);
    assert(_renderer);
    assert(_ui);
    assert(_skybox);

    // Resetting movement tracking
    _movements = glm::ivec3(0);

    static const std::array<void (EventHandler::*)(), NB_IO_EVENTS>
      keyboard_events = {
          &EventHandler::_mouse_exclusive,
          &EventHandler::_close_win_event,
          &EventHandler::_toggle_fullscreen,
          &EventHandler::_jump,
          &EventHandler::_crouch,
          &EventHandler::_front,
          &EventHandler::_back,
          &EventHandler::_right,
          &EventHandler::_left,
          &EventHandler::_left_mouse,
          &EventHandler::_middle_mouse,
          &EventHandler::_right_mouse,
          &EventHandler::_show_fps,
          &EventHandler::_position_info,
          &EventHandler::_display_ui,
          &EventHandler::_about,
          &EventHandler::_invert_camera_y_axis,
          &EventHandler::_particle_position_update,
          &EventHandler::_reset_particles,
      };

    static const std::array<void (EventHandler::*)(), UET_TOTAL_NB>
      ui_events = {
          &EventHandler::_ui_close_app,
          &EventHandler::_ui_mouse_exclusive,
          &EventHandler::_ui_invert_mouse_y_axis,
          &EventHandler::_ui_fullscreen,
          &EventHandler::_ui_pause_start_particles,
          &EventHandler::_ui_reset_simulation,
          &EventHandler::_ui_generate_sphere,
          &EventHandler::_ui_generate_cube,
          &EventHandler::_ui_particle_number,
      };

    // Checking Timers
    auto now = std::chrono::steady_clock::now();
    for (uint32_t i = 0; i < ET_NB_EVENT_TIMER_TYPES; ++i) {
        std::chrono::duration<double> time_diff = now - _timers.time_ref[i];
        _timers.timer_diff[i] = time_diff.count();
        _timers.accept_event[i] = (time_diff.count() > _timers.timer_values[i]);
    }

    // Looping over io events types
    for (uint32_t i = 0; i < NB_IO_EVENTS; ++i) {
        if (ioEvents.events[i]) {
            std::invoke(keyboard_events[i], this);
        }
    }

    // Looping over ui events type
    for (uint32_t i = 0; i < UET_TOTAL_NB; ++i) {
        if (uiEvent.events[i]) {
            std::invoke(ui_events[i], this);
        }
    }

    // Camera updating
    if (_io_manager->isMouseExclusive()) {
        _update_camera(ioEvents.mouse_position);
    }
    _timers.updated[ET_CAMERA] = 1;

    // Skybox
    _skybox->position = _camera->getPosition();
    _renderer->setSkyboxInfo(_skybox->computeInstanceMatrix(glm::vec3(0.0f)));

    // Resized window case
    if (_io_manager->wasResized()) {
        // VK Renderer related
        auto fb_size = _io_manager->getFramebufferSize();
        _renderer->resize(fb_size.x, fb_size.y);

        // Perspective related
        _perspective->ratio = _io_manager->getWindowRatio();
        _camera->setPerspective(
          glm::perspective(glm::radians(_perspective->fov),
                           _perspective->ratio,
                           _perspective->near_far.x,
                           _perspective->near_far.y));
    }

    // Interaction
    _compute_mouse_3d_coordinate(ioEvents.mouse_position);
    if (_timers.updated[ET_LEFT_MOUSE] && !_ui->isUiHovered()) {
        _gravity_center = _mouse_pos_3d;
        _renderer->setParticleGravityCenter(_gravity_center);
    }

    // Ui info
    _ui->setCameraPos(_camera->getPosition());
    _ui->setGravityCenterPos(_gravity_center);
    _ui->setCursorPositionWindow(_mouse_pos_window);
    _ui->setCursorPosition3D(_mouse_pos_3d);

    // Setting timers origin
    for (uint32_t i = 0; i < ET_NB_EVENT_TIMER_TYPES; ++i) {
        if (_timers.updated[i]) {
            _timers.time_ref[i] = now;
        }
        _timers.updated[i] = 0;
    }
}

void
EventHandler::_mouse_exclusive()
{
    if (_timers.accept_event[ET_SYSTEM]) {
        _io_manager->toggleMouseExclusive();
        _mouse_pos_skip = true;
        _ui->toggleCameraMvt();
        _timers.accept_event[ET_SYSTEM] = 0;
        _timers.updated[ET_SYSTEM] = 1;
    }
}

void
EventHandler::_close_win_event()
{
    if (_timers.accept_event[ET_SYSTEM]) {
        _io_manager->triggerClose();
        _timers.accept_event[ET_SYSTEM] = 0;
        _timers.updated[ET_SYSTEM] = 1;
    }
}

void
EventHandler::_toggle_fullscreen()
{
    if (_timers.accept_event[ET_SYSTEM]) {
        _io_manager->toggleFullscreen();
        _ui->toggleFullscreen();
        _timers.accept_event[ET_SYSTEM] = 0;
        _timers.updated[ET_SYSTEM] = 1;
    }
}

void
EventHandler::_jump()
{
    _movements.z += 1;
}

void
EventHandler::_crouch()
{
    _movements.z -= 1;
}

void
EventHandler::_front()
{
    _movements.x += 1;
}

void
EventHandler::_back()
{
    _movements.x -= 1;
}

void
EventHandler::_right()
{
    _movements.y += 1;
}

void
EventHandler::_left()
{
    _movements.y -= 1;
}

void
EventHandler::_left_mouse()
{
    _timers.updated[ET_LEFT_MOUSE] = 1;
}

void
EventHandler::_middle_mouse()
{
    if (_timers.accept_event[ET_MIDDLE_MOUSE]) {
        _timers.accept_event[ET_MIDDLE_MOUSE] = 0;
        _timers.updated[ET_MIDDLE_MOUSE] = 1;
    }
}

void
EventHandler::_right_mouse()
{
    if (_timers.accept_event[ET_RIGHT_MOUSE]) {
        _timers.accept_event[ET_RIGHT_MOUSE] = 0;
        _timers.updated[ET_RIGHT_MOUSE] = 1;
    }
}

void
EventHandler::_show_fps()
{
    if (_timers.accept_event[ET_SYSTEM]) {
        _ui->toggleShowFps();
        _timers.accept_event[ET_SYSTEM] = 0;
        _timers.updated[ET_SYSTEM] = 1;
    }
}

void
EventHandler::_position_info()
{
    if (_timers.accept_event[ET_SYSTEM]) {
        _ui->toggleInfoPosition();
        _timers.accept_event[ET_SYSTEM] = 0;
        _timers.updated[ET_SYSTEM] = 1;
    }
}

void
EventHandler::_display_ui()
{
    if (_timers.accept_event[ET_SYSTEM]) {
        _ui->toggleDisplayUi();
        _timers.accept_event[ET_SYSTEM] = 0;
        _timers.updated[ET_SYSTEM] = 1;
    }
}

void
EventHandler::_about()
{
    if (_timers.accept_event[ET_SYSTEM]) {
        _ui->toggleAbout();
        _timers.accept_event[ET_SYSTEM] = 0;
        _timers.updated[ET_SYSTEM] = 1;
    }
}

void
EventHandler::_invert_camera_y_axis()
{
    if (_timers.accept_event[ET_SYSTEM]) {
        _ui->toggleInvertCameraYAxis();
        _invert_y_axis = !_invert_y_axis;
        _timers.accept_event[ET_SYSTEM] = 0;
        _timers.updated[ET_SYSTEM] = 1;
    }
}

void
EventHandler::_particle_position_update()
{
    if (_timers.accept_event[ET_SYSTEM]) {
        _ui_pause_start_particles();
        _timers.accept_event[ET_SYSTEM] = 0;
        _timers.updated[ET_SYSTEM] = 1;
    }
}

void
EventHandler::_reset_particles()
{
    if (_timers.accept_event[ET_SYSTEM]) {
        _ui_reset_simulation();
        _timers.accept_event[ET_SYSTEM] = 0;
        _timers.updated[ET_SYSTEM] = 1;
    }
}

void
EventHandler::_ui_close_app()
{
    _io_manager->triggerClose();
}

void
EventHandler::_ui_mouse_exclusive()
{
    _io_manager->toggleMouseExclusive();
    _mouse_pos_skip = true;
}

void
EventHandler::_ui_invert_mouse_y_axis()
{
    _invert_y_axis = !_invert_y_axis;
}

void
EventHandler::_ui_fullscreen()
{
    _io_manager->toggleFullscreen();
}

void
EventHandler::_ui_pause_start_particles()
{
    _renderer->toggleUpdateParticlesPosition();
}

void
EventHandler::_ui_reset_simulation()
{
    _renderer->setParticleGravityCenter(
      VulkanRenderer::DEFAULT_PARTICLES_GRAVITY_CENTER);
    _renderer->setParticlesNumber(_ui->getNbParticles());
}

void
EventHandler::_ui_generate_sphere()
{
    _renderer->setParticleGenerationType(VulkanParticleGenerationType::SPHERE);
}

void
EventHandler::_ui_generate_cube()
{
    _renderer->setParticleGenerationType(VulkanParticleGenerationType::CUBE);
}

void
EventHandler::_ui_particle_number()
{
    _renderer->setParticlesNumber(_ui->getNbParticles());
}

void
EventHandler::_update_camera(glm::vec2 const &mouse_pos)
{
    _mouse_pos = mouse_pos;
    if (_mouse_pos_skip) {
        _previous_mouse_pos = _mouse_pos;
        _mouse_pos_skip = false;
    }

    glm::vec2 offset = _mouse_pos - _previous_mouse_pos;
    if (_invert_y_axis) {
        offset.y = -offset.y;
    }
    if (_movements != glm::ivec3(0)) {
        _camera->updatePosition(_movements,
                                _timers.timer_diff[ET_CAMERA] /
                                  _timers.timer_values[ET_CAMERA]);
    }
    if (offset != glm::vec2(0.0)) {
        _camera->updateFront(offset, 0.5f);
        _previous_mouse_pos = _mouse_pos;
    }
    _camera->updateMatrices();
}

void
EventHandler::_compute_mouse_3d_coordinate(glm::vec2 mouse_pos_2d)
{
    static constexpr glm::vec3 const PROJ_SCALE{ 20.0f };

    glm::vec2 win_size{ _io_manager->getWindowSize() };
    glm::vec2 win_center{ win_size / 2.0f };
    _mouse_pos_window =
      (_io_manager->isMouseExclusive()) ? win_center : mouse_pos_2d;

    glm::vec2 ratio{ win_size.x / win_size.y, win_size.y / win_size.x };
    auto pitch = 1.0f / win_size;
    glm::vec2 m{ (_mouse_pos_window - win_center) * pitch * ratio };
    glm::vec3 dx = _camera->getRight() * m.x * PROJ_SCALE;
    glm::vec3 dy = _camera->getUp() * -m.y * PROJ_SCALE;
    _mouse_pos_3d =
      (_camera->getPosition() + dx + dy) + _camera->getFront() * PROJ_SCALE;
}
