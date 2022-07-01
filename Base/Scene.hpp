#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Model.hpp"
#include "Shader.hpp"

namespace RenderIt
{

class Scene
{
  public:
    Scene();

    ~Scene();

    static std::shared_ptr<Scene> Instance();

    void AttachObject(std::shared_ptr<Model> model);

    void Draw(std::shared_ptr<Shader> shader) const;

    void UI();

  public:
    const std::string NAME = "Scene";

  private:
    std::vector<std::shared_ptr<Model>> _models;
};

} // namespace RenderIt
