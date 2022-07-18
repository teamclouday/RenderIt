#include "Camera.hpp"

#include <GL/glew.h>
#include <glm/gtx/transform.hpp>

#include <algorithm>

namespace RenderIt
{

Camera::Camera()
    : clearColor(0.0f, 0.0f, 0.0f, 1.0f), computeShadowData(false), _posVec(0.0f, 0.0f, -1.0f), _centerVec(0.0f),
      _upVec(0.0f), _frontVec(0.0f), _rightVec(0.0f), _worldUpVec(0.0f, 1.0f, 0.0f), _dist(0.0f), _projMat(1.0f),
      _viewMat(1.0f), _viewType(CameraViewType::Persp), _fov(45.0f), _aspect(1.0f), _viewNear(0.1f), _viewFar(1000.0f),
      _updated(false), _csmNearFar(0.01f, 2.0f)
{
    setupCSMBuffer();
    updateCSMDists();
}

std::shared_ptr<Camera> Camera::Instance()
{
    static auto cam = std::make_shared<Camera>();
    return cam;
}

void Camera::PrepareFrame(unsigned clearMask)
{
    glClear(static_cast<GLbitfield>(clearMask));
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    if (!_updated)
        update();
}

const glm::mat4 &Camera::GetView()
{
    return _viewMat;
}

const glm::mat4 &Camera::GetProj()
{
    return _projMat;
}

void Camera::SetPosition(const glm::vec3 &pos)
{
    _posVec = pos;
    _updated = false;
}

void Camera::SetCenter(const glm::vec3 &center)
{
    _centerVec = center;
    _updated = false;
}

void Camera::SetWorldUp(const glm::vec3 &up)
{
    _worldUpVec = up;
    _updated = false;
}

void Camera::SetFov(float fov)
{
    _fov = std::max(0.0f, std::min(180.0f, fov));
    _updated = false;
}

void Camera::SetWindowAspect(int width, int height)
{
    if (!height)
        return;
    _aspect = static_cast<float>(width) / static_cast<float>(height);
    _updated = false;
}

void Camera::SetViewNearFar(float near, float far)
{
    _viewNear = std::max(0.0f, near);
    _viewFar = std::max(_viewNear, far);
    _updated = false;
}

void Camera::SetViewType(CameraViewType type)
{
    _viewType = type;
    _updated = false;
}

glm::vec3 Camera::GetPosition()
{
    return _posVec;
}

glm::vec3 Camera::GetCenter()
{
    return _centerVec;
}

glm::vec3 Camera::GetVecUp()
{
    return _upVec;
}

glm::vec3 Camera::GetVecFront()
{
    return _frontVec;
}

glm::vec3 Camera::GetVecRight()
{
    return _rightVec;
}

CameraViewType Camera::GetViewType()
{
    return _viewType;
}

float Camera::GetFov()
{
    return _fov;
}

void Camera::update()
{
    _frontVec = _centerVec - _posVec;
    _dist = glm::length(_frontVec);
    _frontVec = glm::normalize(_frontVec);
    _rightVec = glm::normalize(glm::cross(_frontVec, _worldUpVec));
    _upVec = glm::normalize(glm::cross(_rightVec, _frontVec));

    _viewMat = glm::lookAt(_posVec, _posVec + _frontVec, _upVec);
    switch (_viewType)
    {
    case CameraViewType::Ortho: {
        float per_depth = std::atan(glm::radians(_fov) / 2.0f);
        float size_y = per_depth * _dist;
        float size_x = _aspect * size_y;
        _projMat = glm::ortho(-size_x, size_x, -size_y, size_y, 0.0f, 2.0f * _dist);
        break;
    }
    case CameraViewType::Persp: {
        _projMat = glm::perspective(glm::radians(_fov), _aspect, _viewNear, _viewFar);
        break;
    }
    }
    if (computeShadowData)
        updateCSMData();
    _updated = true;
}

} // namespace RenderIt
