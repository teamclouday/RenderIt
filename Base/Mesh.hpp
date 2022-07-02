#pragma once
#include <GL/glew.h>

#include <memory>
#include <vector>

#include "GLStructs.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"

namespace RenderIt
{

enum class MeshShape
{
    None = 0,
    Triangle = 1,
    Plane = 2,
    Cube = 3,
    Sphere = 4
};

class Mesh
{
  public:
    Mesh();

    ~Mesh();

    void Draw(std::shared_ptr<Shader> shader) const;

    void Load(MeshShape shape);

    void Load(const std::vector<Vertex> &vertices, const std::vector<unsigned> &indices,
              std::shared_ptr<Material> material, GLenum primType = GL_TRIANGLES);

    void Reset();

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
