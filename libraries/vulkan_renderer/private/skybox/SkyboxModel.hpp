#ifndef PARTICLE_SYSTEM_SKYBOXMODEL_HPP
#define PARTICLE_SYSTEM_SKYBOXMODEL_HPP

#include <array>
#include <vector>
#include <unordered_map>

#include "glm/glm.hpp"

class SkyboxModel final
{
  public:
    SkyboxModel();
    ~SkyboxModel() = default;
    SkyboxModel(SkyboxModel const &src) = delete;
    SkyboxModel &operator=(SkyboxModel const &rhs) = delete;
    SkyboxModel(SkyboxModel &&src) = delete;
    SkyboxModel &operator=(SkyboxModel &&rhs) = delete;

    [[nodiscard]] std::vector<glm::vec3> const &getVertexList() const;
    [[nodiscard]] std::vector<uint32_t> const &getIndicesList() const;

  private:
    static constexpr std::array<glm::vec3, 36> const _skybox_vertices = {
        glm::vec3(-1.0f, 1.0f, -1.0f),  glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f),

        glm::vec3(1.0f, -1.0f, -1.0f),  glm::vec3(1.0f, 1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, -1.0f),

        glm::vec3(-1.0f, -1.0f, 1.0f),  glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, -1.0f),

        glm::vec3(-1.0f, 1.0f, -1.0f),  glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(-1.0f, -1.0f, 1.0f),

        glm::vec3(1.0f, -1.0f, -1.0f),  glm::vec3(1.0f, -1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),

        glm::vec3(1.0f, 1.0f, 1.0f),    glm::vec3(1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f),

        glm::vec3(-1.0f, -1.0f, 1.0f),  glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),

        glm::vec3(1.0f, 1.0f, 1.0f),    glm::vec3(1.0f, -1.0f, 1.0f),
        glm::vec3(-1.0f, -1.0f, 1.0f),

        glm::vec3(-1.0f, 1.0f, -1.0f),  glm::vec3(1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),

        glm::vec3(1.0f, 1.0f, 1.0f),    glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f, -1.0f),

        glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f),

        glm::vec3(1.0f, -1.0f, -1.0f),  glm::vec3(-1.0f, -1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, 1.0f)
    };

    std::vector<glm::vec3> _vertex_list;
    std::vector<uint32_t> _indices_list;
};

#endif // PARTICLE_SYSTEM_SKYBOXMODEL_HPP
