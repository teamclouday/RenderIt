#include "Scene.hpp"

namespace RenderIt
{

Scene::Scene()
{
}

Scene::~Scene()
{
}

Scene *Scene::Instance()
{
    static Scene scene;
    return &scene;
}

} // namespace RenderIt
