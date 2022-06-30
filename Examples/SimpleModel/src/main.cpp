#include "RenderIt.hpp"

#include <cassert>
#include <string>

using namespace RenderIt;

int main()
{
    AppContext *app;
    Camera *cam;
    InputManager *input;

    try
    {
        app = AppContext::Instance();
        cam = Camera::Instance();
        input = InputManager::Instance();
    }
    catch (const std::exception &e)
    {
        display_message("Program", e.what(), MessageType::ERROR);
    }

    int w = 0, h = 0;
    app->SetWindowTitle("Simple Model");
    app->displayUI = false;
    app->EnableCommonGLFeatures();

    // prepare shaders
    auto vertShader = read_file_content("./shaders/SimpleModel.vert");
    auto fragShader = read_file_content("./shaders/SimpleModel.frag");

    auto shader = Shader();
    shader.AddSource(vertShader, GL_VERTEX_SHADER);
    shader.AddSource(fragShader, GL_FRAGMENT_SHADER);
    shader.Compile();
    assert(shader.IsCompiled());

    // load model
    // auto modelPath = select_file_in_explorer();
    std::string modelPath = "./cube/cube.obj";
    // std::string modelPath = "./grey_knight/scene.gltf";
    Model model = Model();
    if (!model.Load(modelPath))
    {
        display_message("Program", "Failed to load model " + modelPath, MessageType::ERROR);
        return -1;
    }

    // setup camera
    cam->SetPosition(glm::vec3(2.0f, 2.0f, 2.0f));
    cam->SetCenter(glm::vec3(0.0f));
    cam->SetFov(60.0f);
    cam->SetViewNearFar(0.01f, 1000.0f);
    cam->SetViewType(CameraViewType::Projection);

    auto mView = cam->GetView();
    auto mProj = cam->GetProj();
    auto mModel = model.transform.GetMatrix();

    while (!app->WindowShouldClose())
    {
        // scene logics

        // render
        app->GetWindowSize(w, h);
        cam->SetWindowAspect(w, h);
        glViewport(0, 0, w, h);
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Bind();
        mView = cam->GetView();
        mProj = cam->GetProj();
        shader.UniformMat4("mvp", mProj * mView * mModel);
        model.Draw(&shader);
        shader.UnBind();

        app->LoopEndFrame();

        // input handling
        if (input->GetKeyPressed(GLFW_KEY_ESCAPE))
            break;
        input->Update();
    }

    return 0;
}
