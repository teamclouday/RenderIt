#pragma once
#include <GL/glew.h>

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "GLStructs.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"

/** @file */

namespace RenderIt
{

/// A general mesh
class Mesh
{
  public:
    Mesh();

    ~Mesh();

    /// Draw mesh data
    void Draw(const Shader *shader) const;

    /// Load with mesh data
    void Load(const std::vector<Vertex> &vertices, const std::vector<unsigned> &indices,
              std::shared_ptr<Material> material, GLenum primType = GL_TRIANGLES);

    /// Reset mesh data
    void Reset();

    /// Get vertex array
    std::optional<GLuint> GetVertexArray();

    /// Get vertex buffer
    std::optional<GLuint> GetVertexBuffer();

    /// Get index buffer
    std::optional<GLuint> GetIndexBuffer();

    /// Get material
    std::shared_ptr<Material> GetMaterial();

    /// Get number of vertices
    size_t GetNumVertices() const;

    /// Get number of indices
    size_t GetNumIndices() const;

    /// UI calls
    void UI();

  public:
    const std::string LOGNAME = "Mesh";

  private:
    /// Configure VAO attribute points
    void setupVAOAttributes() const;

  private:
    std::unique_ptr<SVAO> _vao;
    std::unique_ptr<SBuffer> _vbo;
    std::unique_ptr<SBuffer> _ebo;
    std::shared_ptr<Material> _mat;
    size_t _indicesCount, _verticesCount;
    GLenum _primType;
};

} // namespace RenderIt
