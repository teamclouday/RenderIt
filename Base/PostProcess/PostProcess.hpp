#pragma once
#include <memory>

#include <GL/glew.h>

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

    /// Get framebuffer
    GLuint GetFramebuffer() const
    {
        return _FBO ? _FBO->Get() : 0u;
    }

    /// Read & copy content from other PostProcess framebuffer
    void ReadFramebuffer(const PostProcess &other, GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
                         GLenum filter = GL_LINEAR)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, other.GetFramebuffer());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GetFramebuffer());
        glBlitFramebuffer(0, 0, other._frameWidth, other._frameHeight, 0, 0, _frameWidth, _frameHeight, mask, filter);
    }

    /// UI calls
    void UI();

  protected:
    int _frameWidth, _frameHeight;
    std::unique_ptr<SFBO> _FBO;
};

} // namespace RenderIt
