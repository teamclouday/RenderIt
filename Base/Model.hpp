#pragma once
#include <assimp/postprocess.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Animation.hpp"
#include "Bounds.hpp"
#include "GLStructs.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Transform.hpp"

/** @file */

namespace RenderIt
{

#define MODEL_LOAD_FLAGS                                                                                               \
    aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SplitLargeMeshes |                             \
        aiProcess_PreTransformVertices | aiProcess_ValidateDataStructure | aiProcess_ImproveCacheLocality |            \
        aiProcess_RemoveRedundantMaterials | aiProcess_GenSmoothNormals | aiProcess_FlipUVs |                          \
        aiProcess_FindInvalidData | aiProcess_OptimizeMeshes | aiProcess_CalcTangentSpace

/// Model definition
class Model
{
  public:
    friend class Animator;

    Model();

    ~Model();

    /// Load from local file
    bool Load(const std::string &path, unsigned flags = MODEL_LOAD_FLAGS);

    /// Load with simple shape
    bool Load(MeshShape shape);

    /// Load animation from local file
    bool LoadAnimation(const std::string &path);

    /// Draw all meshes
    void Draw(std::shared_ptr<Shader> shader) const;

    /// Reset model data
    void Reset();

    /// Add Model instance as child, returns true if successful
    bool AddChild(std::shared_ptr<Model> child);

    /// Get parent of current model
    std::shared_ptr<Model> GetParent() const;

    /// Get children by index
    std::shared_ptr<Model> GetChild(unsigned idx) const;

    /// Get number of children of current model
    size_t GetNumChildren() const;

    /// Set current active animation
    void SetActiveAnimation(unsigned idx);

    /// Get number of animations
    size_t GetNumAnimations() const;

    /// Whether model has animation
    bool HasAnimation() const;

    /// UI calls
    void UI();

  public:
    const std::string NAME = "Model";
    Transform transform;
    Bounds bounds;
    std::string modelName;

  private:
    /// Load texture from local path
    std::shared_ptr<STexture> LoadTexture(const std::string &path);

    /// Load texture from memory
    std::shared_ptr<STexture> LoadTexture(unsigned char *pixels, int width, int height, const std::string &name);

  private:
#pragma region model_meshes

    std::vector<std::shared_ptr<Mesh>> _meshes;
    // map filename -> texture pointer
    std::unordered_map<std::string, std::shared_ptr<STexture>> _textures;

#pragma endregion model_meshes

#pragma region model_animations

    unsigned _animationActive;
    // animations
    std::vector<std::shared_ptr<Animation>> _animations;
    // map bone name -> (bone ID, transform matrix)
    std::unordered_map<std::string, std::pair<unsigned, glm::mat4>> _boneInfo;
    // animation node
    std::shared_ptr<Animation::Node> _animNodeRoot;

#pragma endregion model_animations

#pragma region model_hierarchy

    std::shared_ptr<Model> _parent;
    std::vector<std::shared_ptr<Model>> _children;

#pragma endregion model_hierarchy
};

} // namespace RenderIt
