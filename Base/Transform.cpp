#include "Transform.hpp"
#include "Camera.hpp"
#include "GLMIOStream.hpp"

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace RenderIt
{

Transform::Transform(Type t) : position(0.0f), rotation(0.0f), scale(1.0f), type(t)
{
    UpdateMatrix();
}

Transform::Transform(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale, Type t)
{
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
    type = t;
    UpdateMatrix();
}

void Transform::UpdateMatrix()
{
    switch (type)
    {
    case Type::SRT: {
        matrix = glm::scale(glm::mat4(1.0f), scale) *
                 glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z)) *
                 glm::translate(glm::mat4(1.0f), position);
        break;
    }
    case Type::TRS:
    default: {
        matrix = glm::translate(glm::mat4(1.0f), position) *
                 glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z)) *
                 glm::scale(glm::mat4(1.0f), scale);
        break;
    }
    }
    matrixInv = glm::inverse(matrix);
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
