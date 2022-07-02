#pragma once
#include <GL/glew.h>

#include <memory>
#include <string>
#include <vector>

#include "GLStructs.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"

/** @file */

namespace RenderIt
{

/// Simple mesh shapes
enum class MeshShape
{
    None,
    Plane,
    Cube,
    Sphere
};

/// A general mesh
class Mesh
{
  public:
    Mesh();

    ~Mesh();

    /// Draw mesh data
    void Draw(std::shared_ptr<Shader> shader) const;

    /// Load simple shape
    void Load(MeshShape shape);

    /// Load with mesh data
    void Load(const std::vector<Vertex> &vertices, const std::vector<unsigned> &indices,
              std::shared_ptr<Material> material, GLenum primType = GL_TRIANGLES);

    /// Reset mesh data
    void Reset();

    /// UI calls
    void UI();

  public:
    const std::string NAME = "Mesh";

  private:
    std::unique_ptr<SVAO> _vao;
    std::unique_ptr<SBuffer> _vbo;
    std::unique_ptr<SBuffer> _ebo;
    std::shared_ptr<Material> _mat;
    GLsizei _indicesCount;
    GLenum _primType;
};

} // namespace RenderIt

namespace std
{
inline string to_string(const RenderIt::MeshShape &type)
{
    switch (type)
    {
    case RenderIt::MeshShape::None:
        return "None";
    case RenderIt::MeshShape::Plane:
        return "Plane";
    case RenderIt::MeshShape::Cube:
        return "Cube";
    case RenderIt::MeshShape::Sphere:
        return "Sphere";
    default:
        return "";
    }
}
} // namespace std
