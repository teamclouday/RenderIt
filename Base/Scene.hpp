#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_set>

#include "Model.hpp"
#include "RenderPass.hpp"
#include "Shader.hpp"

/** @file */

namespace RenderIt
{

/// A general scene
class Scene
{
  public:
    /// Get singleton
    static std::shared_ptr<Scene> Instance();

#pragma region object_management

    /// Add object to scene, returns true if successful
    bool AttachObject(std::shared_ptr<Model> model);

    /// Remove object, returns true if successful
    bool RemoveObject(const std::shared_ptr<Model> &model);

#pragma endregion object_management

    /// Draw scene, and configure shader for each model
    void Draw(const Shader *shader, const RenderPass &pass = RenderPass::Ordered,
              std::function<void(const Model *, const Shader *)> configModelShader = nullptr) const;

    /// UI calls
    void UI();

  public:
    const std::string LOGNAME = "Scene";
    std::unordered_set<std::shared_ptr<Model>> models;
};

} // namespace RenderIt
