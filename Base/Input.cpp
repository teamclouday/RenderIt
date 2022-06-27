#include "Input.hpp"

namespace RenderIt
{

InputManager::InputManager()
{
}

InputManager::~InputManager()
{
}

InputManager *InputManager::instance()
{
    static InputManager input;
    return &input;
}

} // namespace RenderIt
