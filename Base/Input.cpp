#include "Input.hpp"
#include <GLFW/glfw3.h>

#define LOCKIT const std::lock_guard<std::mutex> lock(_mtx);

namespace RenderIt
{

InputManager::InputManager()
    : _justStarted(true), _mousePosData({0.0f}), _mouseDown({false}), _mouseDown_acc({false}), _wheelData({0.0f})
{
}

std::shared_ptr<InputManager> InputManager::Instance()
{
    static auto input = std::make_shared<InputManager>();
    return input;
}

bool InputManager::GetKeyDown(int key)
{
    LOCKIT
    if (_keyStates.count(key))
        return _keyStates[key];
    else
        return false;
}

bool InputManager::GetKeyPressed(int key)
{
    LOCKIT
    if (_keyStates_acc.count(key))
        return _keyStates_acc[key];
    else
        return false;
}

void InputManager::GetMousePos(float &posX, float &posY)
{
    LOCKIT
    posX = _mousePosData[0];
    posY = _mousePosData[1];
}

void InputManager::GetMousePosOffsets(float &offsetX, float &offsetY)
{
    LOCKIT
    offsetX = _mousePosData[2];
    offsetY = _mousePosData[3];
}

void InputManager::GetWheelOffsets(float &offsetX, float &offsetY)
{
    LOCKIT
    offsetX = _wheelData[0];
    offsetY = _wheelData[1];
}

void InputManager::Update()
{
    LOCKIT
    // update keys
    _keyStates_acc.clear();
    // update mouse offsets
    _mousePosData[2] = _mousePosData[3] = 0.0f;
    // update mouse down accumulated
    _mouseDown_acc[0] = _mouseDown[0];
    _mouseDown_acc[1] = _mouseDown[1];
    _mouseDown_acc[2] = _mouseDown[2];
    _justStarted = true;
}

void InputManager::handle_glfw_key(int key, int action)
{
    LOCKIT
    _keyStates[key] = action != GLFW_RELEASE;
    _keyStates_acc[key] = _keyStates_acc.count(key) ? (_keyStates_acc[key] || _keyStates[key]) : _keyStates[key];
}

void InputManager::handle_glfw_mouse_pos(double posX, double posY)
{
    LOCKIT
    float posXf = static_cast<float>(posX);
    float posYf = static_cast<float>(posY);
    if (_justStarted)
    {
        _justStarted = false;
        _mousePosData[0] = posXf;
        _mousePosData[1] = posYf;
        _mousePosData[2] = _mousePosData[3] = 0.0f;
    }
    else
    {
        _mousePosData[2] += posXf - _mousePosData[0];
        _mousePosData[3] += posYf - _mousePosData[1];
        _mousePosData[0] = posXf;
        _mousePosData[1] = posYf;
    }
}

void InputManager::handle_glfw_mouse_click(int button, int action)
{
    LOCKIT
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT: {
        _mouseDown[0] = action == GLFW_PRESS;
        _mouseDown_acc[0] = _mouseDown_acc[0] || _mouseDown[0];
        break;
    }
    case GLFW_MOUSE_BUTTON_MIDDLE: {
        _mouseDown[1] = action == GLFW_PRESS;
        _mouseDown_acc[1] = _mouseDown_acc[1] || _mouseDown[1];
        break;
    }
    case GLFW_MOUSE_BUTTON_RIGHT: {
        _mouseDown[2] = action == GLFW_PRESS;
        _mouseDown_acc[2] = _mouseDown_acc[2] || _mouseDown[2];
        break;
    }
    }
}

void InputManager::handle_glfw_wheel(double xoffset, double yoffset)
{
    LOCKIT
    _wheelData[0] += static_cast<float>(xoffset);
    _wheelData[1] += static_cast<float>(yoffset);
}

} // namespace RenderIt
