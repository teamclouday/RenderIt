#include "RenderIt.hpp"

#include <memory>
#include <string>

#include <imgui.h>

using namespace RenderIt;

int main()
{
    std::shared_ptr<AppContext> app;
    std::shared_ptr<FreeCamera> cam;
    std::shared_ptr<InputManager> input;
    std::shared_ptr<Animator> anim;
    std::shared_ptr<LightManager> lights;
    std::shared_ptr<ShadowManager> shadows;

    try
    {
        app = AppContext::Instance();
        cam = FreeCamera::Instance();
        input = InputManager::Instance();
        anim = Animator::Instance();
        lights = LightManager::Instance();
        shadows = ShadowManager::Instance();
    }
    catch (const std::exception &e)
    {
        Tools::display_message("Program", e.what(), Tools::MessageType::ERROR);
    }

    int w = 0, h = 0;
    app->SetWindowTitle("Shadow");
    app->displayUI = false;
    app->EnableCommonGLFeatures();
    app->SetVsync(true);
    Tools::set_gl_debug(true);

    // prepare shaders
    auto shader = std::make_shared<Shader>();
    shader->AddSource(Tools::read_file_content("./shaders/Shadow.vert"), GL_VERTEX_SHADER);
    shader->AddSource(Tools::read_file_content("./shaders/Shadow.frag"), GL_FRAGMENT_SHADER);
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

    auto modelPlane = std::make_shared<Model>();
    if (modelPlane->Load(MeshShape::Plane))
    {
        modelPlane->transform.position = glm::vec3(0.0f, -0.5f, 0.0f);
        modelPlane->transform.UpdateMatrix();
    }

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
    // lights->PushLight(LightType::Directional);
    // (reinterpret_cast<DirLight *>(lights->GetLight(LightType::Directional, 0u)))->castShadow = 1;
    lights->PushLight(LightType::Point);
    (reinterpret_cast<PointLight *>(lights->GetLight(LightType::Point, 0u)))->castShadow = 1;
    (reinterpret_cast<PointLight *>(lights->GetLight(LightType::Point, 0u)))->pos = glm::vec3(0.3f, 0.0f, 0.0f);
    lights->Update(true);

    // filters
    auto filterTone = std::make_unique<PostProcessTone>();
    auto filterLum = std::make_unique<PostProcessLuminance>();

    // shadows
    shadows->AttachCamera(cam);
    shadows->AttachLightManager(lights);
    cam->computeShadowData = true;

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
            if (model && modelPlane && ImGui::BeginTabItem("Model"))
            {
                if (ImGui::TreeNode("Model"))
                {
                    model->UI();
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Plane"))
                {
                    modelPlane->UI();
                    ImGui::TreePop();
                }
                ImGui::EndTabItem();
            }
            if (lights && ImGui::BeginTabItem("Lights"))
            {
                lights->UI();
                ImGui::EndTabItem();
            }
            if (filterTone && filterLum && ImGui::BeginTabItem("Tone"))
            {
                ImGui::Text("Filter: %s", filterTone->NAME.c_str());
                ImGui::PushID(filterTone->NAME.c_str());
                filterTone->UI();
                ImGui::PopID();
                ImGui::Text("Filter: %s", filterLum->NAME.c_str());
                ImGui::PushID(filterLum->NAME.c_str());
                filterLum->UI();
                ImGui::PopID();
                ImGui::EndTabItem();
            }
            if (shadows && ImGui::BeginTabItem("Shadows"))
            {
                shadows->UI();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    };

    auto recordShadows = [&](const Shader *shader) {
        anim->BindBones(0u);
        shader->UniformMat4(shadows->ShaderModelName, model->transform.matrix);
        model->Draw(shader, RenderPass::Opaque);
        shader->UniformMat4(shadows->ShaderModelName, modelPlane->transform.matrix);
        modelPlane->Draw(shader);
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

        // record shadows
        shadows->RecordShadows(recordShadows);

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
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lights->DrawLights(mProjView, cam->GetPosition());

        shader->Bind();

        // vertex stage uniforms
        shader->UniformMat4("mat_Model", model->transform.matrix);
        shader->UniformMat3("mat_ModelInv", glm::mat3(model->transform.matrixInv));
        shader->UniformMat4("mat_ProjView", mProjView);
        shader->SsboBinding("BoneMatrices", 0u);
        anim->BindBones(0u);
        // fragment stage uniforms
        shader->SsboBinding("LightsData", 1u);
        shader->UniformVec3("vec_CameraPosWS", cam->GetPosition());
        lights->BindLights(1u);
        // set shadow data
        {
            int texIdx = Material::MAX_MAPS_COUNT;
            shader->UniformInt("map_DirShadow", texIdx);
            shader->TextureBinding(shadows->GetShadowMaps(LightType::Directional), static_cast<unsigned>(texIdx));
            shadows->BindShadowData(LightType::Directional, 2u);
            shadows->BindLightSpaceData(LightType::Directional, 3u);
            texIdx++;
            shader->UniformInt("map_PointShadow", texIdx);
            shader->TextureBinding(shadows->GetShadowMaps(LightType::Point), static_cast<unsigned>(texIdx));
            shader->UniformVec2("shadowPointData", cam->GetOmniShadowNearFar());
        }
        // draw
        model->Draw(shader.get(), RenderPass::AllOrdered);

        shader->UniformMat4("mat_Model", modelPlane->transform.matrix);
        shader->UniformMat3("mat_ModelInv", glm::mat3(modelPlane->transform.matrixInv));
        modelPlane->Draw(shader.get());
        // unbind
        lights->UnBindLights(1u);
        anim->UnBindBones(0u);
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
