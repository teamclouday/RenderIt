#pragma once
#include <functional>
#include <memory>
#include <string>

#include <GL/glew.h>

#include "GLStructs.hpp"
#include "Shader.hpp"

/** @file */

namespace RenderIt
{

/// Abstract Post-Processing Filter
class PostProcess
{
  public:
    /// Start record into framebuffer
    virtual void StartRecord() = 0;

    /// Stop recording in framebuffer
    virtual void StopRecord() = 0;

    /// Draw recorded texture
    virtual void Draw(std::function<void(const Shader *)> func = nullptr) = 0;

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
    void ReadFramebuffer(const PostProcess *other, GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
                         GLenum filter = GL_NEAREST)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, other->GetFramebuffer());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GetFramebuffer());
        glBlitFramebuffer(0, 0, other->_frameWidth, other->_frameHeight, 0, 0, _frameWidth, _frameHeight, mask, filter);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

    void ReadGlobalFramebuffer(int w, int h, GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
                               GLenum filter = GL_NEAREST)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GetFramebuffer());
        glBlitFramebuffer(0, 0, w, h, 0, 0, _frameWidth, _frameHeight, mask, filter);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

    /// UI calls
    void UI();

  public:
    const std::string NAME = "PostProcess";

  protected:
    int _frameWidth, _frameHeight;
    std::unique_ptr<SFBO> _FBO;
};

} // namespace RenderIt
