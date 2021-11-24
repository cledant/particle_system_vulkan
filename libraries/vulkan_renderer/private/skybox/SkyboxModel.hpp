#ifndef PARTICLE_SYSTEM_SKYBOXMODEL_HPP
#define PARTICLE_SYSTEM_SKYBOXMODEL_HPP

#include <array>

#include "glm/glm.hpp"

static constexpr std::array const SKYBOX_VERTICES = {
    glm::vec3(-1, 1, -1), glm::vec3(-1, -1, -1), glm::vec3(1, -1, -1),
    glm::vec3(1, 1, -1),  glm::vec3(-1, -1, 1),  glm::vec3(-1, 1, 1),
    glm::vec3(1, -1, 1),  glm::vec3(1, 1, 1)
};

static constexpr std::array const SKYBOX_INDICES = {
    0, 1, 2, 2, 3, 0, 4, 1, 0, 0, 5, 4, 2, 6, 7, 7, 3, 2,
    4, 5, 7, 7, 6, 4, 0, 3, 7, 7, 5, 0, 1, 4, 2, 2, 4, 6
};

#endif // PARTICLE_SYSTEM_SKYBOXMODEL_HPP
