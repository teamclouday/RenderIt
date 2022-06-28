#pragma once
#include <glm/glm.hpp>

#include <string>

/** @file */

namespace RenderIt
{

/// Defines camera view types
enum class CameraViewType
{
    Ortho = 0,
    Projection = 1
};

/// Camera class for 3D view
class Camera
{
  public:
    Camera();

    /// Get singleton
    static Camera *Instance();

    /// Prepare frame, called before rendering
    void PrepareFrame();

    /// Get view matrix
    const glm::mat4 &GetView();

    /// Get projection matrix
    const glm::mat4 &GetProj();

    /// Set camera position (world space)
    void SetPosition(const glm::vec3 &pos);

    /// Set camera look at center (world space)
    void SetCenter(const glm::vec3 &center);

    /// Set world space UP vector
    void SetWorldUp(const glm::vec3 &up);

    /// Set camera field of view
    void SetFov(float fov);

    /// Set current window aspect ratio
    void SetWindowAspect(int width, int height);

    /// Set view near and far clipping
    void SetViewNearFar(float near, float far);

    /// UI calls
    void UI();

  public:
    const std::string NAME = "Camera";

  private:
    /// Internal update
    void update();

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
