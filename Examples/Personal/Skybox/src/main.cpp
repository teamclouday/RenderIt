#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
    app->SetWindowTitle("Skybox");
    app->displayUI = false;
    app->EnableCommonGLFeatures();
    app->SetVsync(true);
    Tools::set_gl_debug(true);

    // prepare shaders
    auto shader = std::make_shared<Shader>();
    shader->AddSource(Tools::read_file_content("./shaders/Skybox.vert"), GL_VERTEX_SHADER);
    shader->AddSource(Tools::read_file_content("./shaders/Skybox.frag"), GL_FRAGMENT_SHADER);
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

    // add default light
    lights->PushLight(LightType::Directional);

    // filters
    auto filterTone = std::make_unique<PostProcessTone>();
    auto filterLum = std::make_unique<PostProcessLuminance>();

    // skybox
    auto skybox = std::make_unique<Skybox>();
    // skybox->Load("./assets/cape_hill_2k.hdr");
    skybox->Load("./assets/skybox/right.jpg", "./assets/skybox/left.jpg", "./assets/skybox/top.jpg",
                 "./assets/skybox/bottom.jpg", "./assets/skybox/front.jpg", "./assets/skybox/back.jpg");

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
        if (doWireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        lights->Update();

        // render
        app->GetWindowSize(w, h);
        cam->SetWindowAspect(w, h);
        filterTone->Update(w, h);
        filterLum->Update(w, h);
        glViewport(0, 0, w, h);
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mView = cam->GetView();
        mProj = cam->GetProj();
        auto mProjView = mProj * mView;

        filterTone->StartRecord();
        // clear depth & render sky as background
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        skybox->ClearBackground(mProj, mView);

        lights->DrawLights(mProjView, cam->GetPosition());

        shader->Bind();

        // vertex stage uniforms
        shader->UniformMat4("mat_Model", model->transform.matrix);
        shader->UniformMat3("mat_ModelInv", glm::mat3(model->transform.matrixInv));
        shader->UniformMat4("mat_ProjView", mProjView);
        shader->SsboBinding("BoneMatrices", 0);
        anim->BindBones(0);
        // fragment stage uniforms
        shader->SsboBinding("LightsData", 1);
        shader->UniformVec3("vec_CameraPosWS", cam->GetPosition());
        lights->BindLights(1);
        // skybox
        {
            auto texIdx = Material::MAX_MAPS_COUNT;
            shader->UniformInt("map_Skybox", texIdx);
            shader->TextureBinding(skybox->GetSkybox(), static_cast<uint32_t>(texIdx));
        }
        // draw
        model->Draw(shader.get(), RenderPass::AllOrdered);
        // unbind
        lights->UnBindLights(1);
        anim->UnBindBones(0);
        shader->UnBind();

        filterTone->StopRecord();

        // compute average luminance
        filterLum->ReadFramebuffer(filterTone.get(), GL_COLOR_BUFFER_BIT);
        filterLum->Compute(app->GetDeltaTime());
        filterTone->AdjustExposure(filterLum->GetAvgSmoothLuminance());

        // apply tone mapping
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        filterTone->Draw();

        app->LoopEndFrame(renderUI);

        // input handling
        if (input->GetKeyPressed(GLFW_KEY_ESCAPE))
            break;
        if (input->GetKeyPressed(GLFW_KEY_F11))
            app->displayUI = !app->displayUI;
        if (input->GetKeyPressed(GLFW_KEY_SPACE))
            doAnimation = !doAnimation;
        if (input->GetKeyPressed(GLFW_KEY_ENTER))
            doWireframe = !doWireframe;
        input->Update();
    }

    return 0;
}
