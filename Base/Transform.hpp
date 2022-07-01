#pragma once
#include <glm/glm.hpp>

#include "GLStructs.hpp"

namespace RenderIt
{

class Transform
{
  public:
    Transform();

    Transform(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale);

    /// Get model matrix
    glm::mat4 &GetMatrix();

    /// Update model matrix
    void UpdateMatrix();

    /// Given model bounds, compute transformation for unit square around origin
    void TransformToUnitOrigin(const Bounds &b);

    void UI();

  public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

  private:
    glm::mat4 _mat;
};

} // namespace RenderIt
