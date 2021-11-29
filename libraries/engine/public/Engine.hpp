#ifndef PARTICLE_SYS_VULKAN_ENGINE_HPP
#define PARTICLE_SYS_VULKAN_ENGINE_HPP

#include "IOManager.hpp"
#include "Camera.hpp"
#include "EventHandler.hpp"
#include "Perspective.hpp"
#include "VulkanRenderer.hpp"
#include "Ui.hpp"
#include "ModelInstanceInfo.hpp"

class Engine final
{
  public:
    Engine() = default;
    ~Engine() = default;
    Engine(Engine const &src) = delete;
    Engine &operator=(Engine const &rhs) = delete;
    Engine(Engine &&src) = delete;
    Engine &operator=(Engine &&rhs) = delete;

    void init();
    void run();

  private:
    IOManager _io_manager;
    VulkanRenderer _vk_renderer;
    Camera _camera;
    EventHandler _event_handler;
    Perspective _perspective_data{};
    Ui _ui;
    ModelInstanceInfo _skybox{};
};

#endif // PARTICLE_SYS_VULKAN_ENGINE_HPP