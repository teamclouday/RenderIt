#pragma once
#include <glm/glm.hpp>

namespace RenderIt
{

enum class CameraViewType
{
    Ortho = 0,
    Projection = 1
};

class Camera
{
  public:
    Camera();

    static Camera *Instance();

    void PrepareFrame();

    const glm::mat4 &GetView();

    const glm::mat4 &GetProj();

    void SetPosition(const glm::vec3 &pos);

    void SetCenter(const glm::vec3 &center);

    void SetWorldUp(const glm::vec3 &up);

    void SetFov(float fov);

    void SetWindowAspect(int width, int height);

    void SetViewNearFar(float near, float far);

    void UI();

  private:
    void update();

  public:
    const std::string NAME = "Camera";

  private:
    glm::vec4 _clearColor;

    glm::vec3 _posVec, _centerVec;

    glm::vec3 _upVec, _frontVec, _rightVec;
    glm::vec3 _worldUpVec;
    float _dist;

    glm::mat4 _projMat, _viewMat;

    CameraViewType _viewType;
    float _fov, _aspect, _viewNear, _viewFar;

    bool _updated;
};

} // namespace RenderIt
