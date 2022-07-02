#include "Scene.hpp"

namespace RenderIt
{

std::shared_ptr<Scene> Scene::Instance()
{
    static auto scene = std::make_shared<Scene>();
    return scene;
}

void Scene::AttachObject(std::shared_ptr<Model> model)
{
    if (!model)
        return;
    // find the ultimate parent of model
    while (model->GetParent())
        model = model->GetParent();
    _models.insert(model);
}

void Scene::Draw(std::shared_ptr<Shader> shader) const
{
    for (auto &m : _models)
        m->Draw(shader);
}

} // namespace RenderIt
