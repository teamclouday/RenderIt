#include "Cameras/OrbitCamera.hpp"
#include "Input.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>

namespace RenderIt
{

OrbitCamera::OrbitCamera()
    : sensRotateM(1.0f), sensMoveM(0.01f), sensMoveK(0.1f), wheelZoomMinDist(0.1f), _prevMousePos(0.0f)
{
}

std::shared_ptr<OrbitCamera> OrbitCamera::Instance()
{
    static auto cam = std::make_shared<OrbitCamera>();
    return cam;
}

void OrbitCamera::PrepareFrame(unsigned clearMask)
{
    glClear(static_cast<GLbitfield>(clearMask));
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    // update by input
    processMouseMovements();
    processMouseWheel();
    processWASDKeys();
    if (!_updated)
        update();
}

void OrbitCamera::processMouseMovements()
{
    auto input = InputManager::Instance();

    float posX{0.0f}, posY{0.0f};
    input->GetMousePos(posX, posY);

    float offX = posX - _prevMousePos.x;
    float offY = posY - _prevMousePos.y;

    _prevMousePos = glm::vec2(posX, posY);

    bool downLeft = input->GetMouseDown(GLFW_MOUSE_BUTTON_LEFT);
    bool downRight = input->GetMouseDown(GLFW_MOUSE_BUTTON_RIGHT);

    if ((!offX && !offY) || (!downLeft && !downRight))
        return;

    // left mouse down, move camera around center
    if (downLeft)
    {
        offX *= sensRotateM;
        offY *= sensRotateM;
        auto dir = glm::normalize(_posVec - _centerVec);
        dir = glm::vec3(glm::vec4(dir, 0.0f) * glm::rotate(glm::mat4(1.0f), glm::radians(offX), _worldUpVec));
        auto tmp = glm::vec3(glm::vec4(dir, 0.0f) * glm::rotate(glm::mat4(1.0f), glm::radians(offY), _rightVec));
        if (tmp.x * dir.x > 0.00001f)
            dir = tmp;
        _posVec = _centerVec + dir * _dist;
    }
    // right mouse down, move camera parallel to center
    if (downRight)
    {
        offX *= sensMoveM;
        offY *= sensMoveM;
        auto t = glm::translate(glm::mat4(1.0f), offY * _upVec) * glm::translate(glm::mat4(1.0f), -offX * _rightVec);
        _centerVec = glm::vec3(t * glm::vec4(_centerVec, 1.0f));
        _posVec = glm::vec3(t * glm::vec4(_posVec, 1.0f));
    }

    _updated = false;
}

void OrbitCamera::processMouseWheel()
{
    auto input = InputManager::Instance();

    float offX{0.0f}, offY{0.0f};
    input->GetWheelOffsets(offX, offY);

    if (!offY || (_dist + offY <= wheelZoomMinDist))
        return;

    _dist += offY;
    _posVec = _centerVec + glm::normalize(_posVec - _centerVec) * _dist;

    _updated = false;
}

void OrbitCamera::processWASDKeys()
{
    auto input = InputManager::Instance();

    auto offFront = input->GetKeyDown(GLFW_KEY_W) ? 1.0f : 0.0f + input->GetKeyDown(GLFW_KEY_S) ? -1.0f : 0.0f;
    auto offRight = input->GetKeyDown(GLFW_KEY_D) ? 1.0f : 0.0f + input->GetKeyDown(GLFW_KEY_A) ? -1.0f : 0.0f;

    if (!offFront && !offRight)
        return;

    offFront *= sensMoveK;
    offRight *= sensMoveK;
    _posVec += _frontVec * offFront + _rightVec * offRight;

    _updated = false;
}

} // namespace RenderIt
