#pragma once
#include <memory>
#include <string>

#include "Camera.hpp"

#include <glm/glm.hpp>

/** @file */

namespace RenderIt
{

/// Camera that locks to center
class OrbitCamera : public Camera
{
  public:
    OrbitCamera();

    /// Get singleton
    static std::shared_ptr<OrbitCamera> Instance();

    /// Implement new prepare frame method
    void PrepareFrame(unsigned clearMask);

    /// UI calls
    void UI();

  public:
    const std::string LOGNAME = "OrbitCamera";
    // sensitivity of rotation (left mouse drag)
    float sensRotateM;
    // sensitivity of move (right mouse drag)
    float sensMoveM;
    // sensitivity of move (key down)
    float sensMoveK;
    // minimum distance for wheel zoom
    float wheelZoomMinDist;

  private:
    /// Process mouse click & drag
    void processMouseMovements();

    /// Process mouse wheel zoom in/out
    void processMouseWheel();

    /// Process WASD key press
    void processWASDKeys();

  private:
    glm::vec2 _prevMousePos;
};

} // namespace RenderIt
