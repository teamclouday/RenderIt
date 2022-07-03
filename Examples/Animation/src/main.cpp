#include "RenderIt.hpp"

#include <cassert>
#include <memory>
#include <string>

#include <imgui.h>

using namespace RenderIt;

int main()
{
    std::shared_ptr<AppContext> app;
    std::shared_ptr<OrbitCamera> cam;
    std::shared_ptr<InputManager> input;
    std::shared_ptr<Animator> anim;

    try
    {
        app = AppContext::Instance();
        cam = OrbitCamera::Instance();
        input = InputManager::Instance();
        anim = Animator::Instance();
    }
    catch (const std::exception &e)
    {
        Tools::display_message("Program", e.what(), Tools::MessageType::ERROR);
    }

    int w = 0, h = 0;
    app->SetWindowTitle("Animation");
    app->displayUI = false;
    app->EnableCommonGLFeatures();
    app->SetVsync(true);
    Tools::set_gl_debug(true);

    // prepare shaders
    auto vertShader = Tools::read_file_content("./shaders/Animation.vert");
    auto fragShader = Tools::read_file_content("./shaders/Animation.frag");

    auto shader = std::make_shared<Shader>();
    shader->AddSource(vertShader, GL_VERTEX_SHADER);
    shader->AddSource(fragShader, GL_FRAGMENT_SHADER);
    shader->Compile();
    assert(shader->IsCompiled());

    // load model
    auto modelPath = Tools::select_file_in_explorer("Select Model File");
    auto model = std::make_shared<Model>();
    if (!model->Load(modelPath))
    {
        Tools::display_message("Program", "Failed to load model " + modelPath, Tools::MessageType::ERROR);
        return -1;
    }
    model->transform.TransformToUnitOrigin(model->bounds);

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
            if (model && ImGui::BeginTabItem("Model"))
            {
                model->UI();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    };

    app->Start();

    bool doAnimation = true;

    while (!app->WindowShouldClose())
    {
        // do animation
        if (doAnimation)
        {
            anim->Update(app->GetDeltaTime());
            anim->UpdateAnimation(model);
        }

        // render
        app->GetWindowSize(w, h);
        cam->SetWindowAspect(w, h);
        glViewport(0, 0, w, h);
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->Bind();

        mView = cam->GetView();
        mProj = cam->GetProj();
        shader->UniformMat4("mat_model", model->transform.matrix);
        shader->UniformMat3("mat_modelInv", glm::mat3(model->transform.matrixInv));
        shader->UniformMat4("mat_VP", mProj * mView);
        shader->UniformVec3("cameraPosWS", cam->GetPosition());

        shader->UboBinding("BoneMatrices", 0);
        anim->Bind(0);

        model->Draw(shader);

        anim->UnBind();
        shader->UnBind();

        app->LoopEndFrame(renderUI);

        // input handling
        if (input->GetKeyPressed(GLFW_KEY_ESCAPE))
            break;
        if (input->GetKeyPressed(GLFW_KEY_F11))
            app->displayUI = !app->displayUI;
        if (input->GetKeyPressed(GLFW_KEY_SPACE))
            doAnimation = !doAnimation;
        input->Update();
    }

    return 0;
}
