#include "Context.hpp"
#include "Camera.hpp"
#include "Input.hpp"
#include "Tools.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <algorithm>
#include <stdexcept>

namespace RenderIt
{

AppContext::AppContext() : displayUI(true), _winW(800), _winH(600), _winTitle("RenderIt")
{
    initializeLocal();
    initializeGlobal();
}

AppContext::~AppContext()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (_window)
        glfwDestroyWindow(_window);
    glfwTerminate();
}

std::shared_ptr<AppContext> AppContext::Instance()
{
    static auto app = std::make_shared<AppContext>();
    return app;
}

void AppContext::SetWindowSize(int width, int height)
{
    if (!_window)
        return;
    _winW = (std::max)(50, width);
    _winH = (std::max)(50, height);
    glfwSetWindowSize(_window, _winW, _winH);
}

void AppContext::GetWindowSize(int &width, int &height) const
{
    width = _winW;
    height = _winH;
}

void AppContext::SetWindowTitle(const std::string &title)
{
    _winTitle = title;
    glfwSetWindowTitle(_window, _winTitle.c_str());
}

bool AppContext::WindowShouldClose() const
{
    return _window ? glfwWindowShouldClose(_window) : true;
}

void AppContext::LoopEndFrame(std::function<void()> callUI) const
{
    if (displayUI)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (callUI)
            callUI();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    glfwSwapBuffers(_window);
    glfwPollEvents();
}

void AppContext::EnableCommonGLFeatures() const
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void AppContext::initializeLocal()
{
    glfwSetErrorCallback(AppContext::glfw_error_callback);
    if (!glfwInit())
        throw std::runtime_error("Failed to init GLFW!");
    _window = glfwCreateWindow(_winW, _winH, _winTitle.c_str(), nullptr, nullptr);
    if (!_window)
        throw std::runtime_error("Failed to create GLFW window!");
    glfwSetWindowUserPointer(_window, this);
    glfwMakeContextCurrent(_window);

    glfwSetKeyCallback(_window, AppContext::glfw_key_callback);
    glfwSetCursorPosCallback(_window, AppContext::glfw_mousepos_callback);
    glfwSetMouseButtonCallback(_window, AppContext::glfw_mouseclick_callback);
    glfwSetScrollCallback(_window, AppContext::glfw_wheel_callback);
    glfwSetWindowSizeCallback(_window, AppContext::glfw_window_size_callback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        throw std::runtime_error("Failed to init GLEW!");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = "RenderItApp.ini";
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void AppContext::initializeGlobal()
{
    Camera::Instance();
    InputManager::Instance();
}

void AppContext::glfw_error_callback(int error, const char *description)
{
    Tools::display_message("GLFW", std::string(description), Tools::MessageType::ERROR);
}

void AppContext::glfw_window_size_callback(GLFWwindow *window, int width, int height)
{
    auto ctx = reinterpret_cast<AppContext *>(glfwGetWindowUserPointer(window));
    ctx->_winW = width;
    ctx->_winH = height;
}

void AppContext::glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    auto ctx = reinterpret_cast<AppContext *>(glfwGetWindowUserPointer(window));
    auto input = InputManager::Instance();
    input->handle_glfw_key(key, action);
}

void AppContext::glfw_mousepos_callback(GLFWwindow *window, double posX, double posY)
{
    auto ctx = reinterpret_cast<AppContext *>(glfwGetWindowUserPointer(window));
    auto input = InputManager::Instance();
    input->handle_glfw_mouse_pos(posX, posY);
}

void AppContext::glfw_mouseclick_callback(GLFWwindow *window, int button, int action, int mods)
{
    auto ctx = reinterpret_cast<AppContext *>(glfwGetWindowUserPointer(window));
    auto input = InputManager::Instance();
    input->handle_glfw_mouse_click(button, action);
}

void AppContext::glfw_wheel_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    auto ctx = reinterpret_cast<AppContext *>(glfwGetWindowUserPointer(window));
    auto input = InputManager::Instance();
    input->handle_glfw_wheel(xoffset, yoffset);
}

} // namespace RenderIt
