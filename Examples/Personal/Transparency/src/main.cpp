#include "screen.hpp"

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
    app->SetWindowTitle("Transparency");
    app->displayUI = false;
    app->EnableCommonGLFeatures();
    app->SetVsync(true);
    Tools::set_gl_debug(true);

    // prepare shaders
    auto shader = std::make_shared<Shader>();
    shader->AddSource(Tools::read_file_content("./shaders/Transparency.vert"), GL_VERTEX_SHADER);
    shader->AddSource(Tools::read_file_content("./shaders/Transparency.frag"), GL_FRAGMENT_SHADER);
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

    // setup screen record
    auto screen = std::make_unique<ScreenRecorder>();
    auto screenShader = std::make_shared<Shader>();
    screenShader->AddSource(Tools::read_file_content("./shaders/Transparency.vert"), GL_VERTEX_SHADER);
    screenShader->AddSource(Tools::read_file_content("./shaders/TransparencyRefract.frag"), GL_FRAGMENT_SHADER);
    if (!screenShader->Compile())
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

        lights->Update();

        // render
        app->GetWindowSize(w, h);
        cam->SetWindowAspect(w, h);
        screen->Update(w, h);
        glViewport(0, 0, w, h);
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mView = cam->GetView();
        mProj = cam->GetProj();
        auto mProjView = mProj * mView;

        // step 1: render everything except transmissive
        screen->Bind();
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

        screen->UnBind();

        // render recorded as background
        glDisable(GL_DEPTH_TEST);
        screen->Draw();
        glEnable(GL_DEPTH_TEST);
        // copy depth buffer
        screen->screenFBO->BindRead();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        // render transmissive objects
        screenShader->Bind();
        screenShader->UniformMat4("mat_Model", model->transform.matrix);
        screenShader->UniformMat3("mat_ModelInv", glm::mat3(model->transform.matrixInv));
        screenShader->UniformMat4("mat_ProjView", mProjView);
        screenShader->SsboBinding("BoneMatrices", 0);
        screenShader->SsboBinding("LightsData", 1);
        screenShader->UniformVec3("vec_CameraPosWS", cam->GetPosition());
        screenShader->UniformVec3("vec_CameraFrontWS", cam->GetVecFront());
        screenShader->UniformVec2("vec_screenDimInv", glm::vec2(1.0f / w, 1.0f / h));
        screenShader->UniformInt("screenTexture", 10);
        screenShader->TextureBinding(screen->screenTex->Get(), 10u);
        anim->BindBones(0);
        lights->BindLights(1);
        model->Draw(screenShader.get(), RenderPass::Transmissive);
        lights->UnBindLights(1);
        anim->UnBindBones(0);
        screenShader->UnBind();

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
