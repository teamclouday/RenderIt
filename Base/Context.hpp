#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <functional>
#include <memory>
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
    static std::shared_ptr<AppContext> Instance();

    /// Set application window size
    void SetWindowSize(int width, int height);

    /// Get application window size
    void GetWindowSize(int &width, int &height) const;

    /// Set application window title
    void SetWindowTitle(const std::string &title);

    /// Whether window should close
    bool WindowShouldClose() const;

    /// Swap frame buffers, call after rendering
    void LoopEndFrame(std::function<void()> callUI = nullptr);

    /// Enable common OpenGL features
    void EnableCommonGLFeatures() const;

    /// Enable/Disable vsync
    void SetVsync(bool enable) const;

    /// Start by displaying window
    void Start();

    /// Get delta time from last frame (seconds)
    float GetDeltaTime() const;

    /// Get the end time of previous frame
    float GetFrameEndTime() const;

    /// UI calls
    void UI();

  public:
    const std::string LOGNAME = "AppContext";
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
    std::string _vendorInfo;
    std::string _rendererInfo;
    GLFWwindow *_window;
    float _tPrev, _tDelta;
};

} // namespace RenderIt
