#pragma once
#include <array>
#include <mutex>
#include <unordered_map>
#include <utility>

/** @file */

namespace RenderIt
{

/// Input Manager supporting GLFW keys
class InputManager
{
    friend class AppContext;

  public:
    InputManager();

    static InputManager *Instance();

    /// Check if key is down or up
    bool GetKeyDown(int key);

    /// Check if key is pressed once
    bool GetKeyPressed(int key);

    /// Check if mouse button is down
    bool GetMouseDown(int button);

    /// Check if mouse button is pressed once
    bool GetMousePressed(int button);

    /// Get mouse positions
    void GetMousePos(float &posX, float &posY);

    /// Get mouse position offsets
    void GetMousePosOffsets(float &offsetX, float &offsetY);

    /// Get mouse wheel offsets
    void GetWheelOffsets(float &offsetX, float &offsetY);

    /// Call after use of Input states
    void Update();

  private:
#pragma region glfw_callback_handlers

    void handle_glfw_key(int key, int action);

    void handle_glfw_mouse_pos(double posX, double posY);

    void handle_glfw_mouse_click(int button, int action);

    void handle_glfw_wheel(double xoffset, double yoffset);

#pragma endregion glfw_callback_handlers

  private:
    bool _justStarted;
    std::array<float, 4> _mousePosData; // x, y, xoffset, yoffset
    std::array<bool, 3> _mouseDown;     // left, middle, right
    std::array<bool, 3> _mouseDown_acc; // left, middle, right
    std::array<float, 2> _wheelData;    // x, y
    // key code -> whether key is down
    std::unordered_map<int, bool> _keyStates;
    // key code -> whether key has been pressed once
    std::unordered_map<int, bool> _keyStates_acc;

    std::mutex _mtx;
};

} // namespace RenderIt
