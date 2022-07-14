#include "Scene.hpp"

#include <queue>

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

void Scene::Draw(const Shader *shader, const RenderPass &pass,
                 std::function<void(const Model *, const Shader *)> configModelShader) const
{
    auto drawCall = [&](const RenderPass &p) {
        std::queue<const Model *> ms;
        for (auto m : models)
            ms.push(m.get());

        while (!ms.empty())
        {
            auto m = ms.front();
            ms.pop();
            // draw model
            if (configModelShader)
                configModelShader(m, shader);
            m->Draw(shader, p);
            // get children
            for (auto child : m->_children)
                ms.push(child.get());
        }
    };
    switch (pass)
    {
    case RenderPass::Ordered: {
        drawCall(RenderPass::Opaque);
        drawCall(RenderPass::Transparent);
        break;
    }
    default: {
        drawCall(pass);
        break;
    }
    }
}

} // namespace RenderIt
