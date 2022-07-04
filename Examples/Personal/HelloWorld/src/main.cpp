#include "RenderIt.hpp"

#include <cassert>
#include <memory>

using namespace RenderIt;

int main()
{
    std::shared_ptr<AppContext> app;
    std::shared_ptr<Camera> cam;
    std::shared_ptr<InputManager> input;

    try
    {
        app = AppContext::Instance();
        cam = Camera::Instance();
        input = InputManager::Instance();
    }
    catch (const std::exception &e)
    {
        Tools::display_message("Program", e.what(), Tools::MessageType::ERROR);
    }

    int w = 0, h = 0;
    app->SetWindowTitle("Hello World");
    app->displayUI = false;
    app->SetVsync(true);

    // prepare shaders
    auto vertShader = Tools::read_file_content("./shaders/HelloWorld.vert");
    auto fragShader = Tools::read_file_content("./shaders/HelloWorld.frag");
    auto shader = std::make_shared<Shader>();
    shader->AddSource(vertShader, GL_VERTEX_SHADER);
    shader->AddSource(fragShader, GL_FRAGMENT_SHADER);
    shader->Compile();
    assert(shader->IsCompiled());

    // clang-format off
    float vertexData[] {
        -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.0f,  0.5f, 0.0f, 0.0f, 1.0f,
    };
    // clang-format on

    // prepare buffers
    auto vao = SVAO();
    auto vbo = SBuffer(GL_ARRAY_BUFFER);
    vao.Bind();
    vbo.Bind();
    glBufferData(vbo.type, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    vao.UnBind();

    app->Start();

    while (!app->WindowShouldClose())
    {
        // render
        app->GetWindowSize(w, h);
        glViewport(0, 0, w, h);
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT);

        shader->Bind();
        vao.Bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        vao.UnBind();
        shader->UnBind();

        app->LoopEndFrame();

        // input handling
        if (input->GetKeyPressed(GLFW_KEY_ESCAPE))
            break;
        input->Update();
    }

    return 0;
}
