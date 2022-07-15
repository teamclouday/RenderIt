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
    std::shared_ptr<Animator> anim;
    std::shared_ptr<LightManager> lights;

    try
    {
        app = AppContext::Instance();
        cam = OrbitCamera::Instance();
        input = InputManager::Instance();
        anim = Animator::Instance();
        lights = LightManager::Instance();
    }
    catch (const std::exception &e)
    {
        Tools::display_message("Program", e.what(), Tools::MessageType::ERROR);
    }

    int w = 0, h = 0;
    app->SetWindowTitle("MSAA");
    app->displayUI = false;
    app->EnableCommonGLFeatures();
    app->SetVsync(true);
    Tools::set_gl_debug(true);

    glEnable(GL_MULTISAMPLE);

    // prepare shaders
    auto shader = std::make_shared<Shader>();
    shader->AddSource(Tools::read_file_content("./shaders/MSAA.vert"), GL_VERTEX_SHADER);
    shader->AddSource(Tools::read_file_content("./shaders/MSAA.frag"), GL_FRAGMENT_SHADER);
    if (!shader->Compile())
        return -1;

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

    // prepare MSAA
    auto msaa = std::make_unique<PostProcessMSAA>();
    msaa->SetNumSamples(4);

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
            if (lights && ImGui::BeginTabItem("Lights"))
            {
                lights->UI();
                ImGui::EndTabItem();
            }
            if (msaa && ImGui::BeginTabItem("MSAA"))
            {
                msaa->UI();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    };

    app->Start();

    bool doAnimation = true;
    bool doWireframe = false;

    while (!app->WindowShouldClose())
    {
        // do animation
        if (doAnimation)
        {
            anim->Update(app->GetDeltaTime());
            anim->UpdateAnimation(model.get());
        }

        lights->Update();

        // render
        app->GetWindowSize(w, h);
        cam->SetWindowAspect(w, h);
        msaa->Update(w, h);
        glViewport(0, 0, w, h);
        // this prepares default framebuffer
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mView = cam->GetView();
        mProj = cam->GetProj();
        auto mProjView = mProj * mView;

        msaa->StartRecord();
        // this prepares MSAA framebuffer
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->Bind();

        // vertex stage uniforms
        shader->UniformMat4("mat_Model", model->transform.matrix);
        shader->UniformMat3("mat_ModelInv", glm::mat3(model->transform.matrixInv));
        shader->UniformMat4("mat_ProjView", mProjView);
        shader->SsboBinding("BoneMatrices", 0);
        anim->BindBones(0);
        // fragment stage uniforms
        shader->SsboBinding("LightsData", 1);
        lights->BindLights(1);
        shader->UniformVec3("vec_CameraPosWS", cam->GetPosition());

        model->Draw(shader.get());

        lights->UnBindLights(1);
        anim->UnBindBones(0);
        shader->UnBind();

        lights->DrawLights(mProjView, cam->GetPosition());

        msaa->StopRecord();

        msaa->Draw();

        app->LoopEndFrame(renderUI);

        // input handling
        if (input->GetKeyPressed(GLFW_KEY_ESCAPE))
            break;
        if (input->GetKeyPressed(GLFW_KEY_F11))
            app->displayUI = !app->displayUI;
        if (input->GetKeyPressed(GLFW_KEY_SPACE))
            doAnimation = !doAnimation;
        if (input->GetKeyPressed(GLFW_KEY_ENTER))
        {
            doWireframe = !doWireframe;
            if (doWireframe)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        input->Update();
    }

    return 0;
}
