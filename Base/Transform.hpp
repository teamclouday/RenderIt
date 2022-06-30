#pragma once
#include <glm/glm.hpp>

namespace RenderIt
{

class Transform
{
  public:
    Transform();

    Transform(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale);

    glm::mat4 &GetMatrix();

    void Update();

  public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

  private:
    glm::mat4 _mat;
};

} // namespace RenderIt
