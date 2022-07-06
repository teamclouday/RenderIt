#pragma once
#include <iostream>

#include <glm/glm.hpp>

#include "Bounds.hpp"

/** @file */

namespace RenderIt
{

/// Transform info
struct Transform
{
    // Define combination types for computing matrix
    enum class Type
    {
        TRS,
        SRT
    };

    Transform(Type t = Type::SRT);

    Transform(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale, Type t = Type::TRS);

    /// Update model matrix
    void UpdateMatrix();

    /// Reset to default values
    void Reset();

    /// Given model bounds, compute transformation for unit square around origin
    void TransformToUnitOrigin(const Bounds &b);

    friend std::ostream &operator<<(std::ostream &os, const Transform &t);

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::mat4 matrix;
    glm::mat4 matrixInv;

    Type type;
};

} // namespace RenderIt
