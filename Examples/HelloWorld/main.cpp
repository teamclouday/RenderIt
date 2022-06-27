#include "RenderIt.hpp"

using namespace RenderIt;

int main()
{
    AppContext *app;

    try
    {
        app = AppContext::instance();
    }
    catch (const std::exception &e)
    {
        display_message("Program", e.what(), MessageType::ERROR);
    }

    app->setWindowTitle("Hello World");

    while (!app->windowShouldClose())
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.25f, 1.0f, 0.8f, 1.0f);
        app->loopEndFrame();
    }

    return 0;
}
