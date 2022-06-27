#include "RenderIt.hpp"

using namespace RenderIt;

int main()
{
    AppContext *app;
    Camera *cam;

    try
    {
        app = AppContext::Instance();
        cam = Camera::Instance();
    }
    catch (const std::exception &e)
    {
        display_message("Program", e.what(), MessageType::ERROR);
    }

    int w = 0, h = 0;
    app->SetWindowTitle("Hello World");

    while (!app->WindowShouldClose())
    {
        // scene logics

        // render
        app->GetWindowSize(w, h);
        glViewport(0, 0, w, h);
        glClear(GL_COLOR_BUFFER_BIT);
        cam->PrepareFrame();

        app->LoopEndFrame();

        // input handling
    }

    return 0;
}
