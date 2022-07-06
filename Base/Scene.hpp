#pragma once

#include <memory>
#include <set>
#include <string>

#include "Model.hpp"
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

    /// Add object to scene
    void AttachObject(std::shared_ptr<Model> model);

    /// Draw scene
    void Draw(const Shader *shader) const;

    /// UI calls
    void UI();

  public:
    const std::string NAME = "Scene";

  private:
    std::set<std::shared_ptr<Model>> _models;
};

} // namespace RenderIt
