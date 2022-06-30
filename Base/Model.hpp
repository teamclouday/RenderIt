#pragma once
#include <assimp/postprocess.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "GLStructs.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Transform.hpp"

namespace RenderIt
{

#define MODEL_LOAD_FLAGS                                                                                               \
    aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals

class Model
{
  public:
    Model();

    ~Model();

    bool Load(const std::string &path, unsigned flags = MODEL_LOAD_FLAGS);

    bool Load(MeshShape shape);

    void Draw(Shader *shader);

    void Reset();

  public:
    const std::string NAME = "Model";
    Transform transform;

  private:
    std::shared_ptr<STexture> LoadTexture(const std::string &path);

  private:
    Bounds _bounds;
    std::vector<std::shared_ptr<Mesh>> _meshes;
    // map filename -> texture pointer
    std::unordered_map<std::string, std::shared_ptr<STexture>> _textures;
};

} // namespace RenderIt
