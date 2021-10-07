#ifndef SCOP_VULKAN_MODELINSTANCEINFO_HPP
#define SCOP_VULKAN_MODELINSTANCEINFO_HPP

#include "glm/glm.hpp"

struct ModelInstanceInfo final
{
    glm::vec3 position{};
    float pitch{};
    float yaw{};
    float roll{};
    glm::vec3 scale = glm::vec3(1.0f);

    [[nodiscard]] glm::mat4 computeInstanceMatrix(glm::vec3 const &modelCenter) const;
};

#endif // SCOP_VULKAN_MODELINSTANCEINFO_HPP
