#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <functional>
#include <string>

namespace RenderIt
{

class AppContext
{
  public:
    AppContext();

    ~AppContext();

    static AppContext *instance();

    void setWindowSize(int width, int height);

    void getWindowSize(int &width, int &height);

    void setWindowTitle(const std::string &title);

    bool windowShouldClose();

    void loopEndFrame(std::function<void()> callUI = nullptr);

    void UI();

    const std::string NAME = "AppContext";

  public:
    bool displayUI;

  private:
    int _winW, _winH;
    std::string _winTitle;
    GLFWwindow *_window;

  private:
    void initializeLocal();

    void initializeGlobal();

    static void glfw_error_callback(int error, const char *description);

    static void glfw_window_size_callback(GLFWwindow *window, int width, int height);

    static void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void glfw_mousepos_callback(GLFWwindow *window, double posX, double posY);

    static void glfw_mouseclick_callback(GLFWwindow *window, int button, int action, int mods);

    static void glfw_wheel_callback(GLFWwindow *window, double xoffset, double yoffset);
};

} // namespace RenderIt
