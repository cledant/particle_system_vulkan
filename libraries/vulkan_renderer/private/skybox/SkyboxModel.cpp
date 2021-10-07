#include "SkyboxModel.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

SkyboxModel::SkyboxModel()
{
    std::unordered_map<glm::vec3, uint32_t> unique_vertices;
    uint32_t i{};

    for (auto const &it : _skybox_vertices) {

        if (!unique_vertices.contains(it)) {
            unique_vertices[it] = i;
            _vertex_list.emplace_back(it);
            ++i;
        }

        // Indices
        _indices_list.emplace_back(unique_vertices[it]);
    }
}

std::vector<glm::vec3> const &
SkyboxModel::getVertices() const
{
    return (_vertex_list);
}

std::vector<uint32_t> const &
SkyboxModel::getIndicesList() const
{
    return (_indices_list);
}
