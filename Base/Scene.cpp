#include "Scene.hpp"

namespace RenderIt
{

Scene::Scene()
{
}

Scene::~Scene()
{
}

std::shared_ptr<Scene> Scene::Instance()
{
    static auto scene = std::make_shared<Scene>();
    return scene;
}

void Scene::AttachObject(std::shared_ptr<Model> model)
{
    _models.push_back(model);
}

void Scene::Draw(std::shared_ptr<Shader> shader) const
{
    for (auto &m : _models)
        m->Draw(shader);
}

} // namespace RenderIt
