#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <functional>
#include <string>

/** @file */

namespace RenderIt
{

/// Application Context
class AppContext
{
  public:
    AppContext();

    ~AppContext();

    /// Get singleton
    static AppContext *Instance();

    /// Set application window size
    void SetWindowSize(int width, int height);

    /// Get application window size
    void GetWindowSize(int &width, int &height);

    /// Set application window title
    void SetWindowTitle(const std::string &title);

    /// Whether window should close
    bool WindowShouldClose();

    /// Swap frame buffers, call after rendering
    void LoopEndFrame(std::function<void()> callUI = nullptr);

    /// Enable common OpenGL features
    void EnableCommonGLFeatures();

    /// UI calls
    void UI();

  public:
    const std::string NAME = "AppContext";
    bool displayUI;

  private:
    /// Initialize local variables
    void initializeLocal();

    /// Initialize global variables
    void initializeGlobal();

#pragma region glfw_callbacks

    static void glfw_error_callback(int error, const char *description);

    static void glfw_window_size_callback(GLFWwindow *window, int width, int height);

    static void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void glfw_mousepos_callback(GLFWwindow *window, double posX, double posY);

    static void glfw_mouseclick_callback(GLFWwindow *window, int button, int action, int mods);

    static void glfw_wheel_callback(GLFWwindow *window, double xoffset, double yoffset);

#pragma endregion glfw_callbacks

  private:
    int _winW, _winH;
    std::string _winTitle;
    GLFWwindow *_window;
};

} // namespace RenderIt
