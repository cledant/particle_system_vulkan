#include "ModelInstanceInfo.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "glm/gtc/matrix_transform.hpp"

glm::mat4
ModelInstanceInfo::computeInstanceMatrix(glm::vec3 const &modelCenter) const
{
    auto instance_matrix = glm::mat4(1.0f);
    instance_matrix = glm::scale(instance_matrix, scale);
    instance_matrix = glm::translate(instance_matrix, -modelCenter);

    instance_matrix =
      glm::rotate(instance_matrix, pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    instance_matrix =
      glm::rotate(instance_matrix, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    instance_matrix =
      glm::rotate(instance_matrix, roll, glm::vec3(0.0f, 0.0f, 1.0f));
    instance_matrix = glm::translate(instance_matrix, position);

    return (instance_matrix);
}
