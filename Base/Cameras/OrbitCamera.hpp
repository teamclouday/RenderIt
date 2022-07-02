#pragma once
#include <memory>
#include <string>

#include "Camera.hpp"

/** @file */

namespace RenderIt
{

/// Camera that locks to center
class OrbitCamera : public Camera
{
  public:
    /// Get singleton
    static std::shared_ptr<OrbitCamera> Instance();

    /// Implement new prepare frame method
    void PrepareFrame(unsigned clearMask);

  public:
    const std::string NAME = "OrbitCamera";
    // sensitivity of rotation (left mouse drag)
    float sensRotateM = 1.0f;
    // sensitivity of move (right mouse drag)
    float sensMoveM = 0.01f;
    // sensitivity of move (key down)
    float sensMoveK = 0.1f;
    // minimum distance for wheel zoom
    float wheelZoomMinDist = 0.1f;

  private:
    /// Process mouse click & drag
    void ProcessMouseMovements();

    /// Process mouse wheel zoom in/out
    void ProcessMouseWheel();

    /// Process WASD key press
    void ProcessWASDKeys();
};

} // namespace RenderIt
