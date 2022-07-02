#include "RenderIt.hpp"

#include <cassert>
#include <memory>
#include <string>

#include <glm/gtc/matrix_inverse.hpp>
#include <imgui.h>

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
    app->SetWindowTitle("Simple Model");
    app->displayUI = false;
    app->EnableCommonGLFeatures();
    app->SetVsync(true);
    Tools::set_gl_debug(true);

    // prepare shaders
    auto vertShader = Tools::read_file_content("./shaders/SimpleModel.vert");
    auto fragShader = Tools::read_file_content("./shaders/SimpleModel.frag");

    auto shader = std::make_shared<Shader>();
    shader->AddSource(vertShader, GL_VERTEX_SHADER);
    shader->AddSource(fragShader, GL_FRAGMENT_SHADER);
    shader->Compile();
    assert(shader->IsCompiled());

    // load model
    auto modelPath = Tools::select_file_in_explorer();
    auto model = std::make_shared<Model>();
    if (!model->Load(modelPath))
    {
        Tools::display_message("Program", "Failed to load model " + modelPath, Tools::MessageType::ERROR);
        return -1;
    }
    model->transform.TransformToUnitOrigin(model->bounds);

    // setup camera
    cam->SetPosition(glm::vec3(1.0f, 1.0f, 1.0f));
    cam->SetCenter(glm::vec3(0.0f));
    cam->SetFov(45.0f);
    cam->SetViewNearFar(0.01f, 1000.0f);
    cam->SetViewType(CameraViewType::Persp);

    auto mView = cam->GetView();
    auto mProj = cam->GetProj();
    auto mModel = model->transform.matrix;
    auto mModelInv = glm::inverse(mModel);

    app->Start();

    while (!app->WindowShouldClose())
    {
        // render
        app->GetWindowSize(w, h);
        cam->SetWindowAspect(w, h);
        glViewport(0, 0, w, h);
        cam->PrepareFrame(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->Bind();

        mView = cam->GetView();
        mProj = cam->GetProj();
        shader->UniformMat4("mat_model", mModel);
        shader->UniformMat4("mat_modelInv", mModelInv);
        shader->UniformMat4("mat_VP", mProj * mView);
        shader->UniformVec3("cameraPosWS", cam->GetPosition());

        model->Draw(shader);
        shader->UnBind();

        app->LoopEndFrame();

        // input handling
        if (input->GetKeyPressed(GLFW_KEY_ESCAPE))
            break;
        input->Update();
    }

    return 0;
}
