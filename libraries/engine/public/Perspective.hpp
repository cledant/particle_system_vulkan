#ifndef PARTICLE_SYS_VULKAN_PERSPECTIVE_HPP
#define PARTICLE_SYS_VULKAN_PERSPECTIVE_HPP

#include <glm/glm.hpp>

struct Perspective final
{
    float ratio{};
    glm::vec2 near_far{};
    float fov{};
};

#endif // PARTICLE_SYS_VULKAN_PERSPECTIVE_HPP
