#include "Engine.hpp"

#include <cstring>

#include "glm/gtc/matrix_transform.hpp"

#include "AppInfo.hpp"
#include "DefaultParams.hpp"

void
Engine::init()
{
    char engine_name[128] = { 0 };
    std::strcat(engine_name, app_info::APP_NAME);
    std::strcat(engine_name, "_engine");
    IOManagerWindowCreationOption win_opts{
        false, false, false, false, DEFAULT_WIN_SIZE, app_info::APP_NAME
    };

    _event_handler.setCamera(&_camera);
    _event_handler.setIOManager(&_io_manager);
    _event_handler.setPerspectiveData(&_perspective_data);
    _event_handler.setVkRenderer(&_vk_renderer);
    _event_handler.setUi(&_ui);
    _event_handler.setSkybox(&_skybox);
    _io_manager.createWindow(std::move(win_opts));
    _ui.init(_io_manager.getWindow());
    _vk_renderer.createInstance(app_info::APP_NAME,
                                engine_name,
                                VK_MAKE_VERSION(app_info::APP_VERSION_MAJOR,
                                                app_info::APP_VERSION_MINOR,
                                                app_info::APP_VERSION_PATCH),
                                VK_MAKE_VERSION(app_info::APP_VERSION_MAJOR,
                                                app_info::APP_VERSION_MINOR,
                                                app_info::APP_VERSION_PATCH),
                                IOManager::getRequiredInstanceExtension());
    auto fb_size = _io_manager.getFramebufferSize();
    _vk_renderer.init(
      _io_manager.createVulkanSurface(_vk_renderer.getVkInstance()),
      fb_size.x,
      fb_size.y);
    _ui.setNbParticles(DEFAULT_NB_PARTICLES);
    _ui.setParticleMaxSpeed(DEFAULT_PARTICLE_MAX_SPEED);
    _ui.setParticleMass(DEFAULT_PARTICLE_MASS);
    _skybox.scale = glm::vec3(500.0f);
    _perspective_data.near_far = DEFAULT_NEAR_FAR;
    _perspective_data.fov = DEFAULT_FOV;
    _perspective_data.ratio = _io_manager.getWindowRatio();
    _camera.setPerspective(glm::perspective(glm::radians(_perspective_data.fov),
                                            _perspective_data.ratio,
                                            _perspective_data.near_far.x,
                                            _perspective_data.near_far.y));
    _camera.setPosition(START_POS);
    _camera.setYawPitch(START_YAW, START_PITCH);
    _camera.updateMatrices();
}

void
Engine::run()
{
    while (!_io_manager.shouldClose()) {
        _event_handler.processEvents(_io_manager.getEvents(), _ui.getUiEvent());
        _ui.drawUi();
        _vk_renderer.draw(_camera.getPerspectiveViewMatrix());
    }
    _vk_renderer.clear();
    _ui.clear();
    _io_manager.deleteWindow();
}