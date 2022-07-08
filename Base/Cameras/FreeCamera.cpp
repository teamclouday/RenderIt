#include "Cameras/FreeCamera.hpp"
#include "Input.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>

namespace RenderIt
{

FreeCamera::FreeCamera() : sensRotateM(1.0f), sensMoveK(0.01f), wheelZoomMinDist(0.1f), _prevMousePos(0.0f)
{
}

std::shared_ptr<FreeCamera> FreeCamera::Instance()
{
    static auto cam = std::make_shared<FreeCamera>();
    return cam;
}

void FreeCamera::PrepareFrame(unsigned clearMask)
{
    glClear(static_cast<GLbitfield>(clearMask));
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    // update by input
    ProcessMouseMovements();
    ProcessWASDKeys();
    if (!_updated)
        update();
}

void FreeCamera::ProcessMouseMovements()
{
    auto input = InputManager::Instance();

    float posX{0.0f}, posY{0.0f};
    input->GetMousePos(posX, posY);

    float offX = posX - _prevMousePos.x;
    float offY = posY - _prevMousePos.y;

    _prevMousePos = glm::vec2(posX, posY);

    bool downLeft = input->GetMouseDown(GLFW_MOUSE_BUTTON_LEFT);

    if ((!offX && !offY) || !downLeft)
        return;

    // left mouse down, move camera around
    offX *= sensRotateM;
    offY *= sensRotateM;
    auto dir = glm::normalize(_centerVec - _posVec);
    dir = glm::vec3(glm::vec4(dir, 0.0f) * glm::rotate(glm::mat4(1.0f), glm::radians(offX), _worldUpVec));
    auto tmp = glm::vec3(glm::vec4(dir, 0.0f) * glm::rotate(glm::mat4(1.0f), glm::radians(offY), _rightVec));
    if (tmp.x * dir.x > 0.00001f)
        dir = tmp;
    _centerVec = _posVec + dir * _dist;

    _updated = false;
}

void FreeCamera::ProcessWASDKeys()
{
    auto input = InputManager::Instance();

    auto offFront = input->GetKeyDown(GLFW_KEY_W) ? 1.0f : 0.0f + input->GetKeyDown(GLFW_KEY_S) ? -1.0f : 0.0f;
    auto offRight = input->GetKeyDown(GLFW_KEY_D) ? 1.0f : 0.0f + input->GetKeyDown(GLFW_KEY_A) ? -1.0f : 0.0f;

    if (!offFront && !offRight)
        return;

    offFront *= sensMoveK;
    offRight *= sensMoveK;
    auto delta = _frontVec * offFront + _rightVec * offRight;

    _posVec += delta;
    _centerVec += delta;

    _updated = false;
}

} // namespace RenderIt
