#pragma once
#include <memory>

#include "GLStructs.hpp"

namespace RenderIt
{

class PostProcess
{
  public:
    /// Start record into framebuffer
    virtual void StartRecord() = 0;

    /// Stop recording in framebuffer
    virtual void StopRecord() = 0;

    /// Draw recorded texture
    virtual void Draw() = 0;

    /// Update framebuffer & texture dimensions
    virtual bool Update(int screenWidth, int screenHeight) = 0;

    /// Validate framebuffer
    bool IsValid()
    {
        return _FBO && _FBO->Validate();
    }

    /// UI calls
    void UI();

  protected:
    int _frameWidth, _frameHeight;
    std::unique_ptr<SFBO> _FBO;
};

} // namespace RenderIt
