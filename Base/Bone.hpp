#pragma once
#include <string>
#include <utility>
#include <vector>

#include <assimp/anim.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Transform.hpp"

namespace RenderIt
{

struct Bone
{
    Bone(const std::string &boneName, unsigned boneID, const aiNodeAnim *animNode);

    /// Update bone transformation
    void Update(float time);

    /// Interpolate to get position
    glm::vec3 InterpolatePosition(float time);

    /// Interpolate to get rotation
    glm::quat InterpolateRotation(float time);

    /// Interpolate to get scale
    glm::vec3 InterpolateScale(float time);

    // Interpolate between prev, next, and current time
    float Interpolate(float prevTime, float nextTime, float currTime);

    std::vector<std::pair<glm::vec3, float>> positions;
    std::vector<std::pair<glm::quat, float>> rotations;
    std::vector<std::pair<glm::vec3, float>> scales;

    Transform transform;

    std::string name;
    unsigned ID;
};

} // namespace RenderIt
