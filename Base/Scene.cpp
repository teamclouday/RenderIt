#include "Scene.hpp"

namespace RenderIt
{

std::shared_ptr<Scene> Scene::Instance()
{
    static auto scene = std::make_shared<Scene>();
    return scene;
}

bool Scene::AttachObject(std::shared_ptr<Model> model)
{
    if (!model)
        return false;
    // find the ultimate parent of model
    while (model->GetParent())
        model = model->GetParent();
    return models.insert(model).second;
}

bool Scene::RemoveObject(const std::shared_ptr<Model> &model)
{
    if (!models.count(model))
        return false;
    models.erase(model);
    return true;
}

void Scene::Draw(const Shader *shader, std::function<void(const Model *, const Shader *shader)> configModelShader) const
{
    for (auto &m : models)
    {
        if (configModelShader)
            configModelShader(m.get(), shader);
        m->Draw(shader);
    }
}

} // namespace RenderIt
