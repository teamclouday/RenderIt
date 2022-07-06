#pragma once

#include <glm/glm.hpp>

#include <iostream>

/** @file */

namespace RenderIt
{

/// Bounding box (AABB)
struct Bounds
{
    Bounds();

    /// Merge with other bounding box
    void Merge(const Bounds &other);

    /// Update bounding box by position
    void Update(const glm::vec3 &v);

    /// Compute bounding box diagonal length
    float Diagonal() const;

    /// Check if bounding is valid
    bool IsValid() const;

    /// Check & convert to valid values
    void Validate();

    friend std::ostream &operator<<(std::ostream &os, const Bounds &b);

    glm::vec3 max;
    glm::vec3 min;
    glm::vec3 center;
};

} // namespace RenderIt
