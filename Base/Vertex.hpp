#pragma once

#include <glm/glm.hpp>

/** @file */

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

    glm::uvec4 boneIDs;
    glm::vec4 boneWeights;
};

} // namespace RenderIt
