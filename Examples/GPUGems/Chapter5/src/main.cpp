#include "configs.hpp"

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
    app->SetWindowTitle("Chapter 5");
    app->displayUI = false;
    app->EnableCommonGLFeatures();
    app->SetVsync(true);
    Tools::set_gl_debug(true);

    // prepare shaders
    auto shader = std::make_shared<Shader>();
    shader->AddSource(Tools::read_file_content("./shaders/noise.vert"), GL_VERTEX_SHADER);
    shader->AddSource(Tools::read_file_content("./shaders/noise.frag"), GL_FRAGMENT_SHADER);
    shader->AddSource(Tools::read_file_content("./shaders/noise.tese"), GL_TESS_EVALUATION_SHADER);
    shader->AddSource(Tools::read_file_content("./shaders/noise.tesc"), GL_TESS_CONTROL_SHADER);
    if (!shader->Compile())
        return -1;

    // prepare model
    auto model = std::make_unique<Model>();
    if (model->Load(MeshShape::Sphere))
    {
        model->transform.scale = glm::vec3(0.4f);
        model->transform.UpdateMatrix();
        auto mesh = model->GetMesh(0);
        mesh->primType = GL_PATCHES;
    }

    // prepare camera
    cam->SetPosition(glm::vec3(0.0f, 0.0f, 2.0f));
    cam->SetCenter(glm::vec3(0.0f));
    cam->SetFov(45.0f);
    cam->SetViewNearFar(0.01f, 1000.0f);
    cam->SetViewType(CameraViewType::Persp);
    cam->clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

    // configs
    auto configs = std::make_unique<NoiseConfigs>();

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
            if (model && ImGui::BeginTabItem("Model"))
            {
                model->UI();
                ImGui::EndTabItem();
            }
            if (configs && ImGui::BeginTabItem("Configs"))
            {
                configs->UI();
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
    auto mModel = model->transform.matrix;
    auto mModelInv = glm::mat3(model->transform.matrixInv);

    while (!app->WindowShouldClose())
    {
        configs->Update(app->GetFrameEndTime());
        // render
        app->GetWindowSize(w, h);
        cam->SetWindowAspect(w, h);
        glViewport(0, 0, w, h);
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mProj = cam->GetProj();
        mView = cam->GetView();
        mModel = model->transform.matrix;
        mModelInv = glm::mat3(model->transform.matrixInv);
        auto mProjView = mProj * mView;

        shader->Bind();
        shader->UniformFloat("tessLevel", configs->tessLevel);
        shader->UniformInt("controlType", configs->controlType);
        shader->UniformFloat("controlTime", configs->controlTime);
        shader->UniformFloat("controlRes", configs->controlRes);
        shader->UniformVec3("baseColor", configs->baseColor);
        shader->UniformVec3("cameraPos", cam->GetPosition());
        shader->UniformMat4("mat_ProjView", mProjView);
        shader->UniformMat4("mat_Model", mModel);
        shader->UniformMat3("mat_ModelInv", mModelInv);
        model->Draw(shader.get());
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
