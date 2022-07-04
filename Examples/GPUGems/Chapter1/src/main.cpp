#include "RenderIt.hpp"

#include <memory>

using namespace RenderIt;

int main()
{
    std::shared_ptr<AppContext> app;
    std::shared_ptr<InputManager> input;
    std::shared_ptr<Camera> cam;

    try
    {
        app = AppContext::Instance();
        input = InputManager::Instance();
        cam = Camera::Instance();
    }
    catch (const std::exception &e)
    {
        Tools::display_message("Program", e.what(), Tools::MessageType::ERROR);
    }

    int w = 0, h = 0;
    app->SetWindowTitle("GPU Gems Chapter 1");
    app->displayUI = false;
    app->SetVsync(true);
    Tools::set_gl_debug(true);

    cam->SetPosition(glm::vec3(1.0f, 1.0f, 1.0f));
    cam->SetCenter(glm::vec3(0.0f));
    cam->SetFov(45.0f);
    cam->SetViewNearFar(0.01f, 1000.0f);
    cam->SetViewType(CameraViewType::Persp);
    cam->clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

    auto mView = cam->GetView();
    auto mProj = cam->GetProj();

    app->Start();

    while (!app->WindowShouldClose())
    {
        app->GetWindowSize(w, h);
        cam->SetWindowAspect(w, h);
        glViewport(0, 0, w, h);
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        app->LoopEndFrame();

        if (input->GetKeyPressed(GLFW_KEY_ESCAPE))
            break;
        input->Update();
    }

    return 0;
}
