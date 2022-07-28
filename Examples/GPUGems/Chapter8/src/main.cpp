#include "RenderIt.hpp"

#include <memory>
#include <string>

#include <glm/gtc/type_ptr.hpp>
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

    int w = 0, h = 0;
    app->SetWindowTitle("Chapter 8");
    app->displayUI = false;
    app->EnableCommonGLFeatures();
    app->SetVsync(true);
    Tools::set_gl_debug(true);

    // prepare shaders
    auto shader = std::make_shared<Shader>();
    shader->AddSource(Tools::read_file_content("./shaders/Diffraction.vert"), GL_VERTEX_SHADER);
    shader->AddSource(Tools::read_file_content("./shaders/Diffraction.geom"), GL_GEOMETRY_SHADER);
    shader->AddSource(Tools::read_file_content("./shaders/Diffraction.frag"), GL_FRAGMENT_SHADER);
    if (!shader->Compile())
        return -1;

    // setup camera
    cam->SetPosition(glm::vec3(2.0f));
    cam->SetCenter(glm::vec3(0.0f));
    cam->SetFov(45.0f);
    cam->SetViewNearFar(0.01f, 1000.0f);
    cam->SetViewType(CameraViewType::Persp);
    cam->clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

    auto mView = cam->GetView();
    auto mProj = cam->GetProj();

    // filters
    auto filterTone = std::make_unique<PostProcessTone>();
    auto filterLum = std::make_unique<PostProcessLuminance>();

    // skybox
    auto skybox = std::make_unique<Skybox>();
    skybox->Load("./assets/je_gray_02_2k.hdr");

    // configs
    auto configs = glm::vec4(1.0f, 1.0f, 1.0f, 30.0f);
    auto modelTransform = Transform();
    modelTransform.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
    modelTransform.UpdateMatrix();

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
            if (ImGui::BeginTabItem("Transform"))
            {
                auto &t = modelTransform;
                bool shouldUpdate = false;
                shouldUpdate |=
                    ImGui::DragFloat3("Position", glm::value_ptr(t.position), 0.001f, -10000.0f, 10000.0f, "%.3f");
                shouldUpdate |=
                    ImGui::DragFloat3("Rotation", glm::value_ptr(t.rotation), 0.001f, -10000.0f, 10000.0f, "%.3f");
                shouldUpdate |=
                    ImGui::DragFloat3("Scale", glm::value_ptr(t.scale), 0.001f, -10000.0f, 10000.0f, "%.3f");
                if (t.scale.x == t.scale.y && t.scale.x == t.scale.z)
                {
                    if (ImGui::DragFloat("Scale All", &t.scale.x, 0.001f, -10000.0f, 10000.0f, "%.3f"))
                    {
                        t.scale.y = t.scale.x;
                        t.scale.z = t.scale.x;
                        shouldUpdate = true;
                    }
                }
                if (shouldUpdate)
                    t.UpdateMatrix();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Configs"))
            {
                ImGui::DragFloat("Num Sides", &configs.w, 1.0f, 3.0f, 70.0f, "%.0f");
                ImGui::DragFloat("Radius", &configs.z, 0.001f, 0.001f, 10.0f, "%.3f");
                ImGui::DragFloat("r", &configs.x, 0.001f, 0.0f, 10.0f, "%.3f");
                ImGui::DragFloat("d", &configs.y, 0.001f, 0.0f, 10.0f, "%.3f");
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
        filterTone->Update(w, h);
        filterLum->Update(w, h);
        glViewport(0, 0, w, h);
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mView = cam->GetView();
        mProj = cam->GetProj();

        filterTone->StartRecord();
        // clear depth & render sky as background
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        skybox->ClearBackground(mProj, mView);

        shader->Bind();

        shader->UniformMat4("matProjView", mProj * mView);
        shader->UniformMat4("matModel", modelTransform.matrix);
        shader->UniformMat3("matModelInv", glm::mat3(modelTransform.matrixInv));
        shader->UniformFloat("count", configs.w);
        shader->UniformFloat("countInv", 1.0f / configs.w);
        shader->UniformFloat("radius", configs.z);
        shader->UniformVec3("camPosWS", cam->GetPosition());
        shader->UniformFloat("r", configs.x);
        shader->UniformFloat("d", configs.y);
        // skybox
        {
            auto texIdx = Material::MAX_MAPS_COUNT;
            shader->UniformInt("skybox", texIdx);
            shader->TextureBinding(skybox->GetSkybox(), static_cast<uint32_t>(texIdx));
        }
        // draw
        glDisable(GL_CULL_FACE);
        glPointSize(10.0f);
        glDrawArrays(GL_POINTS, 0, 1);
        // unbind
        shader->UnBind();

        filterTone->StopRecord();

        // compute average luminance
        filterLum->ReadFramebuffer(filterTone.get(), GL_COLOR_BUFFER_BIT);
        filterLum->Compute(app->GetDeltaTime());
        filterTone->AdjustExposure(filterLum->GetAvgSmoothLuminance());
        filterTone->Draw();

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
