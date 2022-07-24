#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "RenderIt.hpp"
#include "randgen.hpp"

#include <memory>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

using namespace RenderIt;

#define MAX_GRASS_COUNT 10000

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
    app->SetWindowTitle("Chapter 7");
    app->displayUI = false;
    app->EnableCommonGLFeatures();
    app->SetVsync(true);
    Tools::set_gl_debug(true);

    // prepare camera
    cam->SetPosition(glm::vec3(1.0f));
    cam->SetCenter(glm::vec3(0.0f));
    cam->SetFov(45.0f);
    cam->SetViewNearFar(0.01f, 1000.0f);
    cam->SetViewType(CameraViewType::Persp);
    cam->clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);

    // filters
    auto filterTone = std::make_unique<PostProcessTone>();
    auto filterLum = std::make_unique<PostProcessLuminance>();

    // grass texture
    auto grassTexture = std::make_unique<STexture>(GL_TEXTURE_2D);
    {
        int w, h, n;
        auto grassSource = stbi_load("./assets/grass.png", &w, &h, &n, STBI_rgb_alpha);
        if (grassSource)
        {
            grassTexture->Bind();
            glTexParameteri(grassTexture->type, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(grassTexture->type, GL_TEXTURE_WRAP_R, GL_REPEAT);
            glTexParameteri(grassTexture->type, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(grassTexture->type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(grassTexture->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, grassSource);
            glGenerateMipmap(GL_TEXTURE_2D);
            grassTexture->UnBind();
            stbi_image_free(grassSource);
        }
        else
        {
            Tools::display_message("Program", "Failed to load grass.png (" + std::string(stbi_failure_reason()) + ")",
                                   Tools::MessageType::WARN);
        }
    }

    // grass shader
    auto grassShader = std::make_shared<Shader>();
    grassShader->AddSource(Tools::read_file_content("./shaders/grass.vert"), GL_VERTEX_SHADER);
    grassShader->AddSource(Tools::read_file_content("./shaders/grass.geom"), GL_GEOMETRY_SHADER);
    grassShader->AddSource(Tools::read_file_content("./shaders/grass.frag"), GL_FRAGMENT_SHADER);
    grassShader->Compile();

    // ground
    auto ground = std::make_unique<Model>();
    if (ground->Load(MeshShape::Plane))
    {
        ground->GetMesh(0)->material->twoSided = true;
        ground->GetMesh(0)->material->colorDiffuse = glm::vec3(0.3f, 0.2f, 0.0f);
    }

    // simple shader
    auto simpleShader = std::make_shared<Shader>();
    simpleShader->AddSource(Tools::read_file_content("./shaders/simple.vert"), GL_VERTEX_SHADER);
    simpleShader->AddSource(Tools::read_file_content("./shaders/simple.frag"), GL_FRAGMENT_SHADER);
    simpleShader->Compile();

    // random generator
    auto rnd = std::make_unique<RandomGenerator>();

    // grass offsets
    int numGrass = 200;
    auto rndOffsets = rnd->GeneratePositions(numGrass);
    auto grassSSBO = std::make_unique<SBuffer>(GL_SHADER_STORAGE_BUFFER);
    grassSSBO->Bind();
    glBufferData(grassSSBO->type, MAX_GRASS_COUNT * sizeof(glm::vec2), rndOffsets.data(), GL_DYNAMIC_DRAW);
    grassSSBO->UnBind();

    // wind data
    auto windDir = glm::vec3(1.0f, 0.0f, 0.0f);
    auto windStrength = 1.0f;

    // UI calls
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
            if (ground && ImGui::BeginTabItem("Ground"))
            {
                ground->UI();
                ImGui::EndTabItem();
            }
            if (rnd && ImGui::BeginTabItem("Grass Config"))
            {
                if (ImGui::DragInt("Num Grass", &numGrass, 1, 1, MAX_GRASS_COUNT) ||
                    ImGui::Button("Regenerate Offsets"))
                {
                    auto rndOffsets = rnd->GeneratePositions(numGrass);
                    grassSSBO->Bind();
                    glBufferSubData(grassSSBO->type, 0, numGrass * sizeof(glm::vec2), rndOffsets.data());
                    grassSSBO->UnBind();
                }
                ImGui::Separator();
                ImGui::DragFloat3("Wind Direction", glm::value_ptr(windDir), 0.01f);
                ImGui::DragFloat("Wind Strength", &windStrength, 0.01f, 0.0f, 5.0f, "%.2f");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    };

    app->Start();

    bool doWireframe = false;

    auto mProj = cam->GetProj();
    auto mView = cam->GetView();

    while (!app->WindowShouldClose())
    {
        if (doWireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // render
        app->GetWindowSize(w, h);
        cam->SetWindowAspect(w, h);
        filterTone->Update(w, h);
        filterLum->Update(w, h);
        glViewport(0, 0, w, h);
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        filterTone->StartRecord();
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mProj = cam->GetProj();
        mView = cam->GetView();
        auto mProjView = mProj * mView;

        // render ground
        glDisable(GL_CULL_FACE);
        simpleShader->Bind();
        simpleShader->UniformMat4("matMVP", mProjView * ground->transform.matrix);
        ground->Draw(simpleShader.get());
        simpleShader->UnBind();

        // render grass
        grassShader->Bind();
        grassShader->UniformMat4("matProjView", mProjView);
        grassShader->TextureBinding(grassTexture->Get(), 0u);
        grassShader->UniformInt("grassTexture", 0);
        grassShader->UniformFloat("valTime", app->GetFrameEndTime());
        grassShader->UniformVec3("vecWind", windDir);
        grassShader->UniformFloat("valWind", windStrength);
        grassSSBO->BindBase(1u);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, numGrass);
        grassSSBO->UnBindBase(1u);
        grassShader->UnBind();

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
        if (input->GetKeyPressed(GLFW_KEY_ENTER))
            doWireframe = !doWireframe;
        input->Update();
    }

    return 0;
}
