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
    static constexpr glm::vec2 const DEFAULT_NEAR_FAR =
      glm::vec2(0.1f, 5000.0f);
    static constexpr float const DEFAULT_FOV = 45.0f;
    static constexpr glm::vec3 const START_POS = glm::vec3(-20.0f, 0.0f, 0.0f);
    static constexpr float const START_YAW = 0.0f;
    static constexpr float const START_PITCH = 0.0f;

    IOManager _io_manager;
    VulkanRenderer _vk_renderer;
    Camera _camera;
    EventHandler _event_handler;
    Perspective _perspective_data{};
    Ui _ui;
    ModelInstanceInfo _skybox{};
};

#endif // PARTICLE_SYS_VULKAN_ENGINE_HPP