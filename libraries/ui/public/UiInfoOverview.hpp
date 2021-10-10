#ifndef PARTICLE_SYS_VULKAN_INFO_OVERVIEW_HPP
#define PARTICLE_SYS_VULKAN_INFO_OVERVIEW_HPP

#include <string>

#include "glm/glm.hpp"

class UiInfoOverview final
{
  public:
    UiInfoOverview() = default;
    ~UiInfoOverview() = default;
    UiInfoOverview(UiInfoOverview const &src) = delete;
    UiInfoOverview &operator=(UiInfoOverview const &rhs) = delete;
    UiInfoOverview(UiInfoOverview &&src) = delete;
    UiInfoOverview &operator=(UiInfoOverview &&rhs) = delete;

    void draw(bool &fps, bool &info) const;
    void setAvgFps(float avgFps);
    void setCurrentFps(float currentFps);
    void setCameraPos(glm::vec3 const &cameraPos);
    void setGravityCenterPos(glm::vec3 const &gravityCenterPos);

  private:
    float _avg_fps{};
    float _current_fps{};
    glm::vec3 _camera_pos{};
    glm::vec3 _gravity_center_pos{};
};

#endif // PARTICLE_SYS_VULKAN_INFO_OVERVIEW_HPP
