#include "RenderIt.hpp"

#include <memory>
#include <string>

#include <imgui.h>

using namespace RenderIt;

int main()
{
    std::shared_ptr<AppContext> app;
    std::shared_ptr<OrbitCamera> cam;
    std::shared_ptr<InputManager> input;

    try
    {
        app = AppContext::Instance();
        cam = OrbitCamera::Instance();
        input = InputManager::Instance();
    }
    catch (const std::exception &e)
    {
        Tools::display_message("Program", e.what(), Tools::MessageType::ERROR);
    }

    int w{0}, h{0};
    app->SetWindowTitle("Simple Shapes");
    app->displayUI = false;
    app->EnableCommonGLFeatures();
    app->SetVsync(true);
    Tools::set_gl_debug(true);

    // prepare shaders
    auto vertShader = Tools::read_file_content("./shaders/SimpleShapes.vert");
    auto fragShader = Tools::read_file_content("./shaders/SimpleShapes.frag");

    auto shader = std::make_shared<Shader>();
    shader->AddSource(vertShader, GL_VERTEX_SHADER);
    shader->AddSource(fragShader, GL_FRAGMENT_SHADER);
    if (!shader->Compile())
        return -1;

    // setup scene
    auto scene = std::make_unique<Scene>();

    // shape 1 plane
    auto shapePlane = std::make_shared<Model>();
    shapePlane->Load(MeshShape::Plane);
    shapePlane->transform.position.y = -1.0f;
    shapePlane->transform.UpdateMatrix();
    scene->AttachObject(shapePlane);

    // setup camera
    cam->SetPosition(glm::vec3(1.0f, 1.0f, 1.0f));
    cam->SetCenter(glm::vec3(0.0f));
    cam->SetFov(45.0f);
    cam->SetViewNearFar(0.01f, 1000.0f);
    cam->SetViewType(CameraViewType::Persp);
    cam->clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

    auto mView = cam->GetView();
    auto mProj = cam->GetProj();

    // define UI
    auto renderUI = [&]() {
        ImGui::Begin("UI");
        if (ImGui::BeginTabBar(""))
        {
            if (app && ImGui::BeginTabItem("Application"))
            {
                app->UI();
                ImGui::EndTabItem();
            }
            if (cam && ImGui::BeginTabItem("Camera"))
            {
                cam->UI();
                ImGui::EndTabItem();
            }
            if (scene && ImGui::BeginTabItem("Scene"))
            {
                scene->UI();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    };

    app->Start();

    while (!app->WindowShouldClose())
    {
        // render
        app->GetWindowSize(w, h);
        cam->SetWindowAspect(w, h);
        glViewport(0, 0, w, h);
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->Bind();

        mView = cam->GetView();
        mProj = cam->GetProj();

        scene->Draw(shader.get());

        shader->UnBind();

        app->LoopEndFrame(renderUI);

        // input handling
        if (input->GetKeyPressed(GLFW_KEY_ESCAPE))
            break;
        if (input->GetKeyPressed(GLFW_KEY_F11))
            app->displayUI = !app->displayUI;
        input->Update();
    }

    return 0;
}
