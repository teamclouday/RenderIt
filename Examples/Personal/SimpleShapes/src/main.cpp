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

    int w{0}, h{0};
    app->SetWindowTitle("Simple Shapes");
    app->displayUI = false;
    app->EnableCommonGLFeatures();
    app->SetVsync(true);
    Tools::set_gl_debug(true);

    // prepare shaders
    auto vertShader = Tools::read_file_content("./shaders/SimpleShapes.vert");
    auto fragShader = Tools::read_file_content("./shaders/SimpleShapes.frag");

    auto shader = std::make_shared<Shader>();
    shader->AddSource(vertShader, GL_VERTEX_SHADER);
    shader->AddSource(fragShader, GL_FRAGMENT_SHADER);
    if (!shader->Compile())
        return -1;

    // setup scene
    auto scene = std::make_unique<Scene>();

    // shape 1 plane
    auto shapePlane = std::make_shared<Model>();
    if (shapePlane->Load(MeshShape::Plane))
    {
        shapePlane->transform.position.y = -0.5f;
        shapePlane->transform.scale = glm::vec3(2.0f);
        shapePlane->transform.UpdateMatrix();
        auto mat = shapePlane->GetMesh(0)->material;
        mat->colorAmbient = glm::vec3(0.25f);
        mat->colorDiffuse = glm::vec3(0.0f, 1.0f, 1.0f);
        mat->colorSpecular = glm::vec3(1.0f);
        mat->valShininess = 32.0f;
        scene->AttachObject(shapePlane);
    }

    // shape 2 cube
    auto shapeCube = std::make_shared<Model>();
    if (shapeCube->Load(MeshShape::Cube))
    {
        shapeCube->transform.position.x = 1.0f;
        shapeCube->transform.scale = glm::vec3(0.2f);
        shapeCube->transform.UpdateMatrix();
        auto mat = shapeCube->GetMesh(0)->material;
        mat->colorAmbient = glm::vec3(0.25f);
        mat->colorDiffuse = glm::vec3(1.0f, 0.0f, 1.0f);
        mat->colorSpecular = glm::vec3(1.0f);
        mat->valShininess = 32.0f;
        scene->AttachObject(shapeCube);
    }

    // shape 3 sphere
    auto shapeSphere = std::make_shared<Model>();
    if (shapeSphere->Load(MeshShape::Sphere))
    {
        shapeSphere->transform.position.x = -1.0f;
        shapeSphere->transform.scale = glm::vec3(0.2f);
        shapeSphere->transform.UpdateMatrix();
        auto mat = shapeSphere->GetMesh(0)->material;
        mat->colorAmbient = glm::vec3(0.25f);
        mat->colorDiffuse = glm::vec3(1.0f, 0.0f, 0.0f);
        mat->colorSpecular = glm::vec3(1.0f);
        mat->valShininess = 32.0f;
        scene->AttachObject(shapeSphere);
    }

    // shape 4 cylinder
    auto shapeCylinder = std::make_shared<Model>();
    if (shapeCylinder->Load(MeshShape::Cylinder))
    {
        shapeCylinder->transform.position.z = 1.0f;
        shapeCylinder->transform.scale = glm::vec3(0.2f);
        shapeCylinder->transform.UpdateMatrix();
        auto mat = shapeCylinder->GetMesh(0)->material;
        mat->colorAmbient = glm::vec3(0.25f);
        mat->colorDiffuse = glm::vec3(0.0f, 1.0f, 0.0f);
        mat->colorSpecular = glm::vec3(1.0f);
        mat->valShininess = 32.0f;
        scene->AttachObject(shapeCylinder);
    }

    // shape 5 cone
    auto shapeCone = std::make_shared<Model>();
    if (shapeCone->Load(MeshShape::Cone))
    {
        shapeCone->transform.position.z = -1.0f;
        shapeCone->transform.scale = glm::vec3(0.2f);
        shapeCone->transform.UpdateMatrix();
        auto mat = shapeCone->GetMesh(0)->material;
        mat->colorAmbient = glm::vec3(0.25f);
        mat->colorDiffuse = glm::vec3(0.0f, 0.0f, 1.0f);
        mat->colorSpecular = glm::vec3(1.0f);
        mat->valShininess = 32.0f;
        scene->AttachObject(shapeCone);
    }

    // shape 6 torus
    auto shapeTorus = std::make_shared<Model>();
    if (shapeTorus->Load(MeshShape::Torus))
    {
        shapeTorus->transform.scale = glm::vec3(0.4f);
        shapeTorus->transform.UpdateMatrix();
        auto mat = shapeTorus->GetMesh(0)->material;
        mat->colorAmbient = glm::vec3(0.25f);
        mat->colorDiffuse = glm::vec3(1.0f, 1.0f, 0.0f);
        mat->colorSpecular = glm::vec3(1.0f);
        mat->valShininess = 32.0f;
        scene->AttachObject(shapeTorus);
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
            ImGui::EndTabBar();
        }
        ImGui::End();
    };

    // define shader configure
    auto configModelShader = [&](const Model *model, const Shader *shader) {
        shader->UniformMat4("mat_Model", model->transform.matrix);
        shader->UniformMat3("mat_ModelInv", glm::mat3(model->transform.matrixInv));
    };

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
        shader->UniformMat4("mat_ProjView", mProj * mView);
        shader->UniformVec3("vec_CameraPosWS", cam->GetPosition());

        scene->Draw(shader.get(), configModelShader);

        shader->UnBind();

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
