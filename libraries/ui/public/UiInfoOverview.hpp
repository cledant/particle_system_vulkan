#ifndef PARTICLE_SYS_VULKAN_INFO_OVERVIEW_HPP
#define PARTICLE_SYS_VULKAN_INFO_OVERVIEW_HPP

#include <string>

#include "glm/glm.hpp"

struct UiInfoOverview final
{
    float avgFps{};
    float currentFps{};
    glm::vec3 cameraPos{};
    glm::vec3 gravityCenterPos{};
    uint32_t nbParticles{};
    uint32_t maxSpeedParticle{};
    glm::vec2 cursorPositionWindow{};
    glm::vec3 cursorPosition3D{};
    float particleMass{};

    void draw(bool &fps, bool &info) const;
};

#endif // PARTICLE_SYS_VULKAN_INFO_OVERVIEW_HPP
