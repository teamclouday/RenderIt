#pragma once
#include <iostream>

#include <glm/glm.hpp>

#include "Bounds.hpp"

namespace RenderIt
{

struct Transform
{
    Transform();

    Transform(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale);

    /// Update model matrix
    void UpdateMatrix();

    /// Given model bounds, compute transformation for unit square around origin
    void TransformToUnitOrigin(const Bounds &b);

    friend std::ostream &operator<<(std::ostream &os, const Transform &t);

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::mat4 matrix;
};

} // namespace RenderIt
