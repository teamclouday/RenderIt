#pragma once
#include <memory>
#include <string>

#include "Camera.hpp"

namespace RenderIt
{

class FreeCamera : public Camera
{
  public:
    /// Get singleton
    static std::shared_ptr<FreeCamera> Instance();

    /// Implement new prepare frame method
    void PrepareFrame(unsigned clearMask);

  public:
    const std::string NAME = "FreeCamera";
    // sensitivity of rotation (left mouse drag)
    float sensRotateM = 1.0f;
    // sensitivity of move (key down)
    float sensMoveK = 0.01f;
    // minimum distance for wheel zoom
    float wheelZoomMinDist = 0.1f;

  private:
    void ProcessMouseMovements();

    void ProcessWASDKeys();
};

} // namespace RenderIt
