#include "Transform.hpp"
#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace RenderIt
{

Transform::Transform() : position(0.0f), rotation(0.0f), scale(1.0f)
{
    UpdateMatrix();
}

Transform::Transform(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale)
{
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
    UpdateMatrix();
}

glm::mat4 &Transform::GetMatrix()
{
    return _mat;
}

void Transform::UpdateMatrix()
{
    _mat = glm::scale(glm::mat4(1.0f), scale) * glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z) *
           glm::translate(glm::mat4(1.0f), position);
}

void Transform::TransformToUnitOrigin(const Bounds &b)
{
    auto len = b.Diagonal();
    if (!b.IsValid() || !len)
        return;
    // position
    position = -b.center;
    // scale
    scale = glm::vec3(1.0f / len);
    UpdateMatrix();
}

std::ostream &operator<<(std::ostream &os, const Transform &t)
{
    os << "T(" << t.position << "|" << t.rotation << "|" << t.scale << ")";
    return os;
}

} // namespace RenderIt
