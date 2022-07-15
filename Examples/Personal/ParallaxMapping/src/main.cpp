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
    std::shared_ptr<LightManager> lights;

    try
    {
        app = AppContext::Instance();
        cam = OrbitCamera::Instance();
        input = InputManager::Instance();
        lights = LightManager::Instance();
    }
    catch (const std::exception &e)
    {
        Tools::display_message("Program", e.what(), Tools::MessageType::ERROR);
    }

    int w = 0, h = 0;
    app->SetWindowTitle("Parallax Mapping");
    app->displayUI = false;
    app->EnableCommonGLFeatures();
    app->SetVsync(true);
    Tools::set_gl_debug(true);

    // prepare shaders
    auto vertShader = Tools::read_file_content("./shaders/Parallax.vert");
    auto fragShader = Tools::read_file_content("./shaders/Parallax.frag");

    auto shader = std::make_shared<Shader>();
    shader->AddSource(vertShader, GL_VERTEX_SHADER);
    shader->AddSource(fragShader, GL_FRAGMENT_SHADER);
    if (!shader->Compile())
        return -1;

    // setup camera
    cam->SetPosition(glm::vec3(1.0f, 1.0f, 1.0f));
    cam->SetCenter(glm::vec3(0.0f));
    cam->SetFov(45.0f);
    cam->SetViewNearFar(0.01f, 1000.0f);
    cam->SetViewType(CameraViewType::Persp);
    cam->clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

    auto mView = cam->GetView();
    auto mProj = cam->GetProj();

    // load texture map
    auto diffMap = std::make_shared<STexture>(GL_TEXTURE_2D);
    {
        int w, h, n;
        auto diffMapSource = stbi_load("./assets/bricks2.jpg", &w, &h, &n, 4);
        if (diffMapSource)
        {
            diffMap->Bind();
            glTexParameteri(diffMap->type, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(diffMap->type, GL_TEXTURE_WRAP_R, GL_REPEAT);
            glTexParameteri(diffMap->type, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(diffMap->type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(diffMap->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, diffMapSource);
            glGenerateMipmap(GL_TEXTURE_2D);
            diffMap->UnBind();
            stbi_image_free(diffMapSource);
        }
        else
        {
            Tools::display_message("Program", "Failed to load bricks2.jpg (" + std::string(stbi_failure_reason()) + ")",
                                   Tools::MessageType::WARN);
        }
    }
    auto normMap = std::make_shared<STexture>(GL_TEXTURE_2D);
    {
        int w, h, n;
        auto normMapSource = stbi_load("./assets/bricks2_normal.jpg", &w, &h, &n, 4);
        if (normMapSource)
        {
            normMap->Bind();
            glTexParameteri(normMap->type, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(normMap->type, GL_TEXTURE_WRAP_R, GL_REPEAT);
            glTexParameteri(normMap->type, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(normMap->type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(normMap->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, normMapSource);
            glGenerateMipmap(GL_TEXTURE_2D);
            normMap->UnBind();
            stbi_image_free(normMapSource);
        }
        else
        {
            Tools::display_message("Program",
                                   "Failed to load bricks2_normal.jpg (" + std::string(stbi_failure_reason()) + ")",
                                   Tools::MessageType::WARN);
        }
    }
    auto heightMap = std::make_shared<STexture>(GL_TEXTURE_2D);
    {
        int w, h, n;
        auto heightMapSource = stbi_load("./assets/bricks2_disp.jpg", &w, &h, &n, 1);
        if (heightMapSource)
        {
            heightMap->Bind();
            glTexParameteri(heightMap->type, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(heightMap->type, GL_TEXTURE_WRAP_R, GL_REPEAT);
            glTexParameteri(heightMap->type, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(heightMap->type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(heightMap->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, heightMapSource);
            glGenerateMipmap(GL_TEXTURE_2D);
            heightMap->UnBind();
            stbi_image_free(heightMapSource);
        }
        else
        {
            Tools::display_message("Program",
                                   "Failed to load bricks2_disp.jpg (" + std::string(stbi_failure_reason()) + ")",
                                   Tools::MessageType::WARN);
        }
    }

    auto heightScale = 0.1f;
    auto heightLayers = 16.0f;

    // load model
    auto scene = std::make_unique<Scene>();
    auto model1 = std::make_shared<Model>();
    if (model1->Load(MeshShape::Cube))
    {
        model1->transform.position.x = -0.5f;
        model1->transform.scale = glm::vec3(0.4f);
        model1->transform.UpdateMatrix();
        auto mat = model1->GetMesh(0)->material;
        mat->colorDiffuse = glm::vec3(1.0);
        mat->colorSpecular = glm::vec3(1.0);
        mat->valShininess = 32.0f;
        mat->diffuse = diffMap;
        mat->normals = normMap;
        mat->height = heightMap;
        scene->AttachObject(model1);
    }
    auto model2 = std::make_shared<Model>();
    if (model2->Load(MeshShape::Cube))
    {
        model2->transform.position.x = 0.5f;
        model2->transform.scale = glm::vec3(0.4f);
        model2->transform.UpdateMatrix();
        auto mat = model2->GetMesh(0)->material;
        mat->colorDiffuse = glm::vec3(1.0);
        mat->colorSpecular = glm::vec3(1.0);
        mat->valShininess = 32.0f;
        mat->diffuse = diffMap;
        mat->normals = normMap;
        scene->AttachObject(model2);
    }

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
            if (lights && ImGui::BeginTabItem("Lights"))
            {
                lights->UI();
                ImGui::EndTabItem();
            }
            if (lights && ImGui::BeginTabItem("Height"))
            {
                ImGui::DragFloat("Scale", &heightScale, 0.001f, 0.0f, 1.0f, "%.3f");
                ImGui::DragFloat("Layers", &heightLayers, 1.0f, 0.0f, 50.0f, "%.0f");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    };

    lights->PushLight(LightType::Directional);

    auto configModelShader = [](const Model *model, const Shader *shader) {
        shader->UniformMat4("mat_Model", model->transform.matrix);
        shader->UniformMat3("mat_ModelInv", glm::mat3(model->transform.matrixInv));
    };

    app->Start();

    bool doWireframe = false;

    while (!app->WindowShouldClose())
    {
        lights->Update();

        // render
        app->GetWindowSize(w, h);
        cam->SetWindowAspect(w, h);
        glViewport(0, 0, w, h);
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mView = cam->GetView();
        mProj = cam->GetProj();
        auto mProjView = mProj * mView;

        lights->DrawLights(mProjView, cam->GetPosition());

        shader->Bind();

        // vertex stage uniforms
        shader->UniformMat4("mat_ProjView", mProjView);
        // fragment stage uniforms
        shader->SsboBinding("LightsData", 1);
        lights->BindLights(1);
        shader->UniformVec3("vec_CameraPosWS", cam->GetPosition());
        shader->UniformFloat("val_HeightScale", heightScale);
        shader->UniformFloat("val_HeightLayers", heightLayers);

        scene->Draw(shader.get(), RenderPass::Ordered, configModelShader);

        lights->UnBindLights(1);
        shader->UnBind();

        app->LoopEndFrame(renderUI);

        // input handling
        if (input->GetKeyPressed(GLFW_KEY_ESCAPE))
            break;
        if (input->GetKeyPressed(GLFW_KEY_F11))
            app->displayUI = !app->displayUI;
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
