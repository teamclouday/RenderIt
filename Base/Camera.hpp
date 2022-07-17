#pragma once
#include <glm/glm.hpp>

#include <array>
#include <memory>
#include <string>

#include "GLStructs.hpp"

#define SHADOW_CSM_COUNT 4

/** @file */

namespace RenderIt
{

/// Defines camera view types
enum class CameraViewType
{
    Ortho = 0,
    Persp = 1
};

/// Camera class for 3D view
class Camera
{
    friend class ShadowManager;

  public:
    Camera();

    /// Get singleton
    static std::shared_ptr<Camera> Instance();

    /// Prepare frame, called before rendering
    void PrepareFrame(unsigned clearMask);

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

    /// Set camera view type
    void SetViewType(CameraViewType type);

    /// Get camera position (world space)
    glm::vec3 GetPosition();

    /// Get camera look at center (world space)
    glm::vec3 GetCenter();

    /// Get camera up vector
    glm::vec3 GetVecUp();

    /// Get camera front vector
    glm::vec3 GetVecFront();

    /// Get camera right vector
    glm::vec3 GetVecRight();

    /// Get camera view type
    CameraViewType GetViewType();

    /// Get camera field of view
    float GetFov();

    /// UI calls
    void UI();

  public:
    const std::string LOGNAME = "Camera";
    glm::vec4 clearColor;
    bool computeShadowData;

  protected:
    /// Internal update
    void update();

  private:
#pragma region cascaded_shadow
    void setupCSMData();

    void updateCSMData();
#pragma endregion cascaded_shadow

  protected:
    glm::vec3 _posVec, _centerVec;

    glm::vec3 _upVec, _frontVec, _rightVec;
    glm::vec3 _worldUpVec;
    float _dist;

    glm::mat4 _projMat, _viewMat;

    CameraViewType _viewType;
    float _fov, _aspect, _viewNear, _viewFar;

    bool _updated;

  private:
#pragma region cascaded_shadow
    std::array<glm::vec2, SHADOW_CSM_COUNT> _csmDistData;
    std::array<glm::vec4, SHADOW_CSM_COUNT> _csmSphereData;
    std::array<glm::vec3[8], SHADOW_CSM_COUNT> _csmFrustumData;
    std::unique_ptr<SBuffer> _csmDataSSBO;
#pragma endregion cascaded_shadow
};

} // namespace RenderIt

namespace std
{
inline string to_string(const RenderIt::CameraViewType &type)
{
    switch (type)
    {
    case RenderIt::CameraViewType::Ortho:
        return "Orthogonal";
    case RenderIt::CameraViewType::Persp:
        return "Perspective";
    default:
        return "";
    }
}
} // namespace std
