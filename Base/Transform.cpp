#include "Transform.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace RenderIt
{

Transform::Transform() : position(0.0f), rotation(0.0f), scale(1.0f)
{
    Update();
}

Transform::Transform(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale)
{
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
    Update();
}

glm::mat4 &Transform::GetMatrix()
{
    return _mat;
}

void Transform::Update()
{
    _mat = glm::translate(glm::mat4(1.0f), position) * glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z) *
           glm::scale(glm::mat4(1.0f), scale);
}

} // namespace RenderIt
