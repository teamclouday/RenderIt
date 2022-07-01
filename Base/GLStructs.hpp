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

// Structure for Frame Buffer Object (FBO)
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

/// Vertex data
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoords;
    glm::vec4 color;
};

/// Material info
struct Material
{
    ~Material()
    {
        diffuse = specular = ambient = emissive = height = normals = shininess = opacity = displacement = nullptr;
        pbr_color = pbr_normal = pbr_emission = pbr_metalness = pbr_roughness = pbr_occlusion = nullptr;
    }

    bool HasPBR()
    {
        return pbr_color || pbr_normal || pbr_emission || pbr_metalness || pbr_roughness || pbr_occlusion;
    }

    // legacy materials
    std::shared_ptr<STexture> diffuse = nullptr;
    std::shared_ptr<STexture> specular = nullptr;
    std::shared_ptr<STexture> ambient = nullptr;
    std::shared_ptr<STexture> emissive = nullptr;
    std::shared_ptr<STexture> height = nullptr;
    std::shared_ptr<STexture> normals = nullptr;
    std::shared_ptr<STexture> shininess = nullptr;
    std::shared_ptr<STexture> opacity = nullptr;
    std::shared_ptr<STexture> displacement = nullptr;
    std::shared_ptr<STexture> lightmap = nullptr;
    std::shared_ptr<STexture> reflection = nullptr;

    inline static const std::string mapNameDiffuse = "map_DIFFUSE";
    inline static const std::string mapNameSpecular = "map_SPECULAR";
    inline static const std::string mapNameAmbient = "map_AMBIENT";
    inline static const std::string mapNameEmissive = "map_EMISSIVE";
    inline static const std::string mapNameHeight = "map_HEIGHT";
    inline static const std::string mapNameNormals = "map_NORMALS";
    inline static const std::string mapNameShininess = "map_SHININESS";
    inline static const std::string mapNameOpacity = "map_OPACITY";
    inline static const std::string mapNameDisplacement = "map_DISPLACEMENT";
    inline static const std::string mapNameLightmap = "map_LIGHTMAP";
    inline static const std::string mapNameReflection = "map_REFLECTION";

    // pbr materials
    std::shared_ptr<STexture> pbr_color = nullptr;
    std::shared_ptr<STexture> pbr_normal = nullptr;
    std::shared_ptr<STexture> pbr_emission = nullptr;
    std::shared_ptr<STexture> pbr_metalness = nullptr;
    std::shared_ptr<STexture> pbr_roughness = nullptr;
    std::shared_ptr<STexture> pbr_occlusion = nullptr;

    inline static const std::string mapNamePBRColor = "mapPBR_COLOR";
    inline static const std::string mapNamePBRNormal = "mapPBR_NORMAL";
    inline static const std::string mapNamePBREmission = "mapPBR_EMISSION";
    inline static const std::string mapNamePBRMetalness = "mapPBR_METALNESS";
    inline static const std::string mapNamePBRRoughness = "mapPBR_ROUGHNESS";
    inline static const std::string mapNamePBROcclusion = "mapPBR_OCCLUSION";

    // this line will be appended to any names
    // to indicate whether the corresponding texture exists
    // e.g. bool map_DIFFUSE_exists;
    inline static const std::string existsEXT = "_exists";
};

/// Bounding Box
struct Bounds
{
    Bounds() : max(std::numeric_limits<float>::min()), min(std::numeric_limits<float>::max()), center(0.0f)
    {
    }

    /// Merge with other bounding box
    void Merge(const Bounds &other)
    {
        max = glm::max(max, other.max);
        min = glm::min(min, other.min);
        center = (max + min) * 0.5f;
    }

    /// Update bounding box by position
    void Update(const glm::vec3 &v)
    {
        max = glm::max(v, max);
        min = glm::min(v, min);
        center = (max + min) * 0.5f;
    }

    /// Compute bounding box diagonal length
    float Diagonal() const
    {
        if (!IsValid())
            return 0.0f;
        return glm::length(max - min);
    }

    /// Check if bounding is valid
    bool IsValid() const
    {
        bool valid = true;
        valid = valid && (max.x > std::numeric_limits<float>::min());
        valid = valid && (max.y > std::numeric_limits<float>::min());
        valid = valid && (max.z > std::numeric_limits<float>::min());

        valid = valid && (min.x < std::numeric_limits<float>::max());
        valid = valid && (min.y < std::numeric_limits<float>::max());
        valid = valid && (min.z < std::numeric_limits<float>::max());

        valid = valid && (max.x >= min.x);
        valid = valid && (max.y >= min.y);
        valid = valid && (max.z >= min.z);
        return valid;
    }

    glm::vec3 max;
    glm::vec3 min;
    glm::vec3 center;
};

inline std::ostream &operator<<(std::ostream &os, const glm::vec2 &v)
{
    os << '<' << v.x << ',' << v.y << '>';
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const glm::vec3 &v)
{
    os << '<' << v.x << ',' << v.y << ',' << v.z << '>';
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const glm::vec4 &v)
{
    os << '<' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << '>';
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const Bounds &b)
{
    os << "B(" << b.max << "|" << b.min << "|" << b.center << ")";
    return os;
}

} // namespace RenderIt
