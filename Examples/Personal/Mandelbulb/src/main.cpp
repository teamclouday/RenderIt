#include "RenderIt.hpp"

#include <cassert>
#include <memory>
#include <string>

#include <imgui.h>

#include "configs.hpp"

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
    app->SetWindowTitle("Mandelbulb");
    app->displayUI = false;
    app->SetVsync(true);
    Tools::set_gl_debug(true);

    // prepare shaders
    auto vertSource = Tools::read_file_content("./shaders/Mandelbulb.vert");
    auto fragSource = Tools::read_file_content("./shaders/Mandelbulb.frag");
    auto shader = std::make_shared<Shader>();
    shader->AddSource(vertSource, GL_VERTEX_SHADER);
    shader->AddSource(fragSource, GL_FRAGMENT_SHADER);
    assert(shader->Compile());

    // setup camera
    cam->SetPosition(glm::vec3(0.0f, 0.0f, -2.0f));
    cam->SetCenter(glm::vec3(0.0f));
    cam->clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

    // clang-format off
    const float vertices[] =
    {
        -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
    };
    // clang-format on

    // prepare vao
    SVAO vao;
    SBuffer vbo(GL_ARRAY_BUFFER);
    vao.Bind();
    vbo.Bind();
    glBufferData(vbo.type, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    vao.UnBind();

    // configs
    MandelbulbConfigs configs;

    // define UI
    auto renderUI = [&]() {
        ImGui::Begin("UI");
        if (ImGui::BeginTabBar(""))
        {
            if (ImGui::BeginTabItem("Configs"))
            {
                configs.UI();
                ImGui::EndTabItem();
            }
            if (cam && ImGui::BeginTabItem("Camera"))
            {
                cam->UI();
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
        glViewport(0, 0, w, h);
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT);

        shader->Bind();
        shader->UniformVec3("val_CameraPos", cam->GetPosition());
        shader->UniformVec3("val_CameraForward", cam->GetVecFront());
        shader->UniformVec3("val_CameraUp", cam->GetVecUp());
        shader->UniformVec3("val_CameraRight", cam->GetVecRight());
        shader->UniformFloat("val_CameraFovRadians", glm::radians(cam->GetFov()));
        shader->UniformFloat("val_CameraRatio", w / static_cast<float>(h));
        shader->UniformFloat("val_RayMarchMaxSteps", configs.marchMaxSteps);
        shader->UniformFloat("val_RayMarchMaxDist", configs.marchMaxDist);
        shader->UniformFloat("val_MMaxDist", configs.mandelbulbMaxDist);
        shader->UniformInt("val_MSteps", configs.mandelbulbSteps);
        shader->UniformFloat("val_Time", app->GetFrameEndTime() / ImGui::GetIO().Framerate);
        vao.Bind();
        glDrawArrays(GL_QUADS, 0, 4);
        vao.UnBind();
        shader->Bind();

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
