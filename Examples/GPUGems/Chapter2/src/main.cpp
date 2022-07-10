
#include "RenderIt.hpp"
#include "configs.hpp"
#include "renderer.hpp"

#include <stb_image.h>

using namespace RenderIt;

int main()
{
    std::shared_ptr<AppContext> app;
    std::shared_ptr<InputManager> input;
    std::shared_ptr<OrbitCamera> cam;

    try
    {
        app = AppContext::Instance();
        input = InputManager::Instance();
        cam = OrbitCamera::Instance();
    }
    catch (const std::exception &e)
    {
        Tools::display_message("Program", e.what(), Tools::MessageType::ERROR);
    }

    // setup app
    int w = 0, h = 0;
    app->SetWindowTitle("GPU Gems Chapter 2");
    app->displayUI = false;
    app->SetVsync(true);
    app->EnableCommonGLFeatures();
    glDisable(GL_CULL_FACE);
    Tools::set_gl_debug(true);

    // setup camera
    cam->SetPosition(glm::vec3(1.0f, 1.0f, 1.0f));
    cam->SetCenter(glm::vec3(0.0f));
    cam->SetFov(45.0f);
    cam->SetViewNearFar(0.01f, 1000.0f);
    cam->SetViewType(CameraViewType::Persp);
    cam->clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

    auto mView = cam->GetView();
    auto mProj = cam->GetProj();

    // setup waves
    auto waveRenderer = std::make_unique<WaveRenderer>();
    auto waveConfigs = std::make_unique<WaveConfigs>();

    // setup ocean ground
    auto waveGround = std::make_unique<Model>();
    if (waveGround->Load(MeshShape::Plane))
    {
        waveGround->transform.position.y = -0.5f;
        waveGround->transform.UpdateMatrix();
        auto mat = waveGround->GetMesh(0)->GetMaterial();
        mat->twoSided = true;
        mat->colorDiffuse = glm::vec3(0.6f, 0.5f, 0.3f);
    }
    auto waveGroundShader = std::make_shared<Shader>();
    waveGroundShader->AddSource(Tools::read_file_content("./shaders/ground.vert"), GL_VERTEX_SHADER);
    waveGroundShader->AddSource(Tools::read_file_content("./shaders/ground.frag"), GL_FRAGMENT_SHADER);
    waveGroundShader->Compile();

    // load lightmap
    auto lightMap = std::make_unique<STexture>(GL_TEXTURE_2D);
    {
        int w, h, n;
        auto lightMapSource = stbi_load("./assets/sun.png", &w, &h, &n, STBI_rgb_alpha);
        if (lightMapSource)
        {
            lightMap->Bind();
            glTexParameteri(lightMap->type, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(lightMap->type, GL_TEXTURE_WRAP_R, GL_REPEAT);
            glTexParameteri(lightMap->type, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(lightMap->type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(lightMap->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, lightMapSource);
            glGenerateMipmap(GL_TEXTURE_2D);
            lightMap->UnBind();
            stbi_image_free(lightMapSource);
        }
        else
        {
            Tools::display_message("Program", "Failed to load sun.jpg (" + std::string(stbi_failure_reason()) + ")",
                                   Tools::MessageType::WARN);
        }
    }

    // UI
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
            if (waveConfigs && ImGui::BeginTabItem("Wave Configs"))
            {
                waveConfigs->UI();
                ImGui::EndTabItem();
            }
            if (waveGround && ImGui::BeginTabItem("Wave Ground"))
            {
                waveGround->UI();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    };

    app->Start();

    while (!app->WindowShouldClose())
    {
        app->GetWindowSize(w, h);
        cam->SetWindowAspect(w, h);
        glViewport(0, 0, w, h);
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mView = cam->GetView();
        mProj = cam->GetProj();

        auto mProjView = mProj * mView;

        // render ground
        {
            waveGroundShader->Bind();
            lightMap->Bind();
            glActiveTexture(GL_TEXTURE0);
            waveGroundShader->UniformInt("lightMap", 0);
            waveGroundShader->UniformMat4("mat_ProjView", mProjView);
            waveGroundShader->UniformMat4("mat_Model", waveGround->transform.matrix);
            waveGroundShader->UniformMat3("mat_ModelInv", glm::mat3(waveGround->transform.matrixInv));

            waveGroundShader->UniformFloat("val_K", waveConfigs->powerConstant);
            waveGroundShader->UniformFloat("val_Time", app->GetFrameEndTime());
            waveGroundShader->UniformVec3("surfColor", waveConfigs->color);

            waveGroundShader->SsboBinding("SineWaves", 0);
            waveConfigs->wavesBuffer->BindBase(0);

            waveGround->Draw(waveGroundShader.get());

            waveConfigs->wavesBuffer->UnBindBase(0);
            lightMap->UnBind();
            waveGroundShader->UnBind();
        }

        // render waves
        {
            auto waveShader = waveRenderer->shader;
            waveShader->Bind();
            waveShader->UniformFloat("tessLevel", waveConfigs->tessLevel);
            waveShader->UniformMat4("mat_ProjView", mProjView);
            waveShader->UniformMat4("mat_Model", waveConfigs->transform.matrix);
            waveShader->UniformMat3("mat_ModelInv", glm::mat3(waveConfigs->transform.matrixInv));
            waveShader->UniformFloat("val_K", waveConfigs->powerConstant);
            waveShader->UniformFloat("val_Time", app->GetFrameEndTime());

            waveShader->UniformVec3("surfColor", waveConfigs->color);
            waveShader->UniformFloat("surfAlpha", waveConfigs->surfaceAlpha);
            waveShader->UniformVec3("cameraPosWS", cam->GetPosition());

            waveShader->SsboBinding("SineWaves", 0);
            waveConfigs->wavesBuffer->BindBase(0);

            waveRenderer->Draw();

            waveConfigs->wavesBuffer->UnBindBase(0);
            waveShader->UnBind();
        }

        app->LoopEndFrame(renderUI);

        if (input->GetKeyPressed(GLFW_KEY_ESCAPE))
            break;
        if (input->GetKeyPressed(GLFW_KEY_F11))
            app->displayUI = !app->displayUI;
        input->Update();
    }

    return 0;
}
