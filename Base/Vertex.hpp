#pragma once

#include <glm/glm.hpp>

namespace RenderIt
{

/// Vertex data
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

} // namespace RenderIt
