#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Model.hpp"
#include "Shader.hpp"

/** @file */

namespace RenderIt
{

/// A general scene
class Scene
{
  public:
    Scene();

    ~Scene();

    /// Get singleton
    static std::shared_ptr<Scene> Instance();

    /// Add object to scene
    void AttachObject(std::shared_ptr<Model> model);

    /// Draw scene
    void Draw(std::shared_ptr<Shader> shader) const;

    /// UI calls
    void UI();

  public:
    const std::string NAME = "Scene";

  private:
    std::vector<std::shared_ptr<Model>> _models;
};

} // namespace RenderIt
