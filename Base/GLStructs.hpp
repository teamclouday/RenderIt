#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

/** @file */

namespace RenderIt
{

/// Structure for Vertex Array Object (VAO)
struct SVAO
{
    SVAO(size_t size = 1)
    {
        IDs.resize(size);
        glGenVertexArrays(static_cast<GLsizei>(size), IDs.data());
    }
    ~SVAO()
    {
        glDeleteVertexArrays(static_cast<GLsizei>(IDs.size()), IDs.data());
    }
    void Bind(size_t idx = 0) const
    {
        if (idx < IDs.size())
            glBindVertexArray(IDs[idx]);
    }
    void UnBind() const
    {
        glBindVertexArray(0);
    }
    GLuint Get(size_t idx = 0) const
    {
        if (IDs.size() > idx)
            return IDs[idx];
        return 0;
    }
    std::vector<GLuint> IDs;
};

/// Structure for OpenGL Buffers
struct SBuffer
{
    SBuffer(GLenum type = GL_ARRAY_BUFFER, size_t size = 1)
    {
        this->type = type;
        IDs.resize(size);
        glGenBuffers(static_cast<GLsizei>(size), IDs.data());
    }
    ~SBuffer()
    {
        glDeleteBuffers(static_cast<GLsizei>(IDs.size()), IDs.data());
    }
    void Bind(size_t idx = 0) const
    {
        if (idx < IDs.size())
            glBindBuffer(type, IDs[idx]);
    }
    void UnBind() const
    {
        glBindBuffer(type, 0);
    }
    void BindBase(GLuint binding, size_t idx = 0) const
    {
        if (idx < IDs.size())
            glBindBufferBase(type, binding, IDs[idx]);
    }
    void UnBindBase(GLuint binding) const
    {
        glBindBufferBase(type, binding, 0);
    }
    GLuint Get(size_t idx = 0) const
    {
        if (IDs.size() > idx)
            return IDs[idx];
        return 0;
    }
    std::vector<GLuint> IDs;
    GLenum type;
};

/// Structure for OpenGL Textures
struct STexture
{
    STexture(GLenum type = GL_TEXTURE_2D, size_t size = 1)
    {
        this->type = type;
        IDs.resize(size);
        glGenTextures(static_cast<GLsizei>(size), IDs.data());
    }
    ~STexture()
    {
        glDeleteTextures(static_cast<GLsizei>(IDs.size()), IDs.data());
    }
    void Bind(size_t idx = 0) const
    {
        if (idx < IDs.size())
            glBindTexture(type, IDs[idx]);
    }
    void UnBind() const
    {
        glBindTexture(type, 0);
    }
    GLuint Get(size_t idx = 0) const
    {
        if (IDs.size() > idx)
            return IDs[idx];
        return 0;
    }
    std::vector<GLuint> IDs;
    GLenum type;
};

/// Structure for Frame Buffer Object (FBO)
struct SFBO
{
    SFBO(size_t size = 1)
    {
        IDs.resize(size);
        glGenFramebuffers(static_cast<GLsizei>(size), IDs.data());
    }
    ~SFBO()
    {
        glDeleteFramebuffers(static_cast<GLsizei>(IDs.size()), IDs.data());
    }
    void Bind(size_t idx = 0) const
    {
        if (idx < IDs.size())
            glBindFramebuffer(GL_FRAMEBUFFER, IDs[idx]);
    }
    void UnBind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void BindRead(size_t idx = 0) const
    {
        if (idx < IDs.size())
            glBindFramebuffer(GL_READ_FRAMEBUFFER, IDs[idx]);
    }
    void UnBindRead() const
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    }
    void BindDraw(size_t idx = 0) const
    {
        if (idx < IDs.size())
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, IDs[idx]);
    }
    void UnBindDraw() const
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
    bool Validate(size_t idx = 0) const
    {
        if (idx >= IDs.size())
            return false;
        return glCheckNamedFramebufferStatus(IDs[idx], GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }
    GLuint Get(size_t idx = 0) const
    {
        if (IDs.size() > idx)
            return IDs[idx];
        return 0;
    }
    std::vector<GLuint> IDs;
};

/// Structure for Render Buffer Object (RBO)
struct SRBO
{
    SRBO(size_t size = 1)
    {
        IDs.resize(size);
        glGenRenderbuffers(static_cast<GLsizei>(size), IDs.data());
    }
    ~SRBO()
    {
        glDeleteRenderbuffers(static_cast<GLsizei>(IDs.size()), IDs.data());
    }
    void Bind(size_t idx = 0) const
    {
        if (idx < IDs.size())
            glBindRenderbuffer(GL_RENDERBUFFER, IDs[idx]);
    }
    void UnBind() const
    {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    GLuint Get(size_t idx = 0) const
    {
        if (IDs.size() > idx)
            return IDs[idx];
        return 0;
    }
    std::vector<GLuint> IDs;
};

} // namespace RenderIt
