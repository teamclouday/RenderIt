#include "RenderIt.hpp"
#include "configs.hpp"

#include <imgui.h>

#include <memory>

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
    app->SetWindowTitle("GPU Gems Chapter 1");
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

    // setup shader
    std::unique_ptr<Shader> shader = std::make_unique<Shader>();
    shader->AddSource(Tools::read_file_content("./shaders/wave.vert"), GL_VERTEX_SHADER);
    shader->AddSource(Tools::read_file_content("./shaders/wave.tesc"), GL_TESS_CONTROL_SHADER);
    shader->AddSource(Tools::read_file_content("./shaders/wave.tese"), GL_TESS_EVALUATION_SHADER);
    shader->AddSource(Tools::read_file_content("./shaders/wave.geom"), GL_GEOMETRY_SHADER);
    shader->AddSource(Tools::read_file_content("./shaders/wave.frag"), GL_FRAGMENT_SHADER);
    if (!shader->Compile())
        return -1;

    // define quad with 2 triangles
    // clang-format off
    const float vertices[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
        -0.5f,  0.5f,
        -0.5f,  0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f
    };
    // clang-format on
    // setup VAO
    auto VAO = SVAO();
    auto VBO = SBuffer(GL_ARRAY_BUFFER);
    VAO.Bind();
    VBO.Bind();
    glBufferData(VBO.type, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    VAO.UnBind();

    // wave configs
    auto configs = std::make_unique<WaveConfigs>();

    // UI func
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
    bool displayNormal = false;
    glLineWidth(2.0f);

    while (!app->WindowShouldClose())
    {
        if (doWireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        app->GetWindowSize(w, h);
        cam->SetWindowAspect(w, h);
        glViewport(0, 0, w, h);
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->Bind();
        shader->UniformFloat("tessLevel", configs->tessLevel);

        mView = cam->GetView();
        mProj = cam->GetProj();
        shader->UniformMat4("mat_VP", mProj * mView);
        shader->UniformMat4("mat_Model", configs->transform.matrix);
        shader->UniformMat3("mat_ModelInv", glm::mat3(configs->transform.matrixInv));
        shader->UniformBool("enableGerstnerWave", configs->enableGerstnerWave);
        shader->UniformFloat("val_K", configs->powerConstant);

        shader->UniformBool("displayNormal", displayNormal || doWireframe);
        shader->UniformVec3("surfColor", configs->color);
        shader->UniformVec3("cameraPosWS", cam->GetPosition());

        shader->SsboBinding("SineWaves", 0);
        shader->UniformFloat("val_Time", app->GetFrameEndTime());
        configs->wavesBuffer->BindBase(0);

        VAO.Bind();
        glDrawArrays(GL_PATCHES, 0, 6);
        VAO.UnBind();

        configs->wavesBuffer->UnBindBase(0);
        shader->UnBind();

        app->LoopEndFrame(renderUI);

        if (input->GetKeyPressed(GLFW_KEY_ESCAPE))
            break;
        if (input->GetKeyPressed(GLFW_KEY_F11))
            app->displayUI = !app->displayUI;
        if (input->GetKeyPressed(GLFW_KEY_SPACE))
            doWireframe = !doWireframe;
        if (input->GetKeyPressed(GLFW_KEY_ENTER))
            displayNormal = !displayNormal;

        input->Update();
    }

    return 0;
}
