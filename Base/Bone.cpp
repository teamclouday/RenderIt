#include "Bone.hpp"
#include "Tools.hpp"

#include <cassert>
#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>

namespace RenderIt
{

Bone::Bone(const std::string &boneName, unsigned boneID, const aiNodeAnim *animNode) : name(boneName), ID(boneID)
{
    assert(animNode);

    assert(animNode->mNumPositionKeys);
    for (auto i = 0; i < animNode->mNumPositionKeys; i++)
    {
        auto position = animNode->mPositionKeys[i].mValue;
        auto timestamp = std::abs(animNode->mPositionKeys[i].mTime);
        positions.push_back({Tools::convertAssimpVector(position), timestamp});
    }

    assert(animNode->mNumRotationKeys);
    for (auto i = 0; i < animNode->mNumRotationKeys; i++)
    {
        auto rotation = animNode->mRotationKeys[i].mValue;
        auto timestamp = std::abs(animNode->mRotationKeys[i].mTime);
        rotations.push_back({Tools::convertAssimpQuaternion(rotation), timestamp});
    }

    assert(animNode->mNumScalingKeys);
    for (auto i = 0; i < animNode->mNumScalingKeys; i++)
    {
        auto scale = animNode->mScalingKeys[i].mValue;
        auto timestamp = std::abs(animNode->mScalingKeys[i].mTime);
        scales.push_back({Tools::convertAssimpVector(scale), timestamp});
    }
}

void Bone::Update(float time)
{
    auto matT = glm::translate(glm::mat4(1.0f), InterpolatePosition(time));
    auto matR = glm::toMat4(InterpolateRotation(time));
    auto matS = glm::scale(glm::mat4(1.0f), InterpolateScale(time));
    matrix = matT * matR * matS;
}

glm::vec3 Bone::InterpolatePosition(float time)
{
    if (positions.size() == 1)
        return positions[0].first;

    // find target index
    unsigned idx = 0;
    for (; idx < positions.size() - 1; idx++)
    {
        if (time < positions[idx + 1].second)
            break;
    }
    // get interpolated position
    auto scale = Interpolate(positions[idx].second, positions[idx + 1].second, time);
    return glm::mix(positions[idx].first, positions[idx + 1].first, scale);
}

glm::quat Bone::InterpolateRotation(float time)
{
    if (rotations.size() == 1)
        return rotations[0].first;

    // find target index
    unsigned idx = 0;
    for (; idx < rotations.size() - 1; idx++)
    {
        if (time < rotations[idx + 1].second)
            break;
    }
    // get interpolated scale
    auto scale = Interpolate(rotations[idx].second, rotations[idx + 1].second, time);
    return glm::slerp(rotations[idx].first, rotations[idx + 1].first, scale);
}

glm::vec3 Bone::InterpolateScale(float time)
{
    if (scales.size() == 1)
        return scales[0].first;

    // find target index
    unsigned idx = 0;
    for (; idx < scales.size() - 1; idx++)
    {
        if (time < scales[idx + 1].second)
            break;
    }
    // get interpolated scale
    auto scale = Interpolate(scales[idx].second, scales[idx + 1].second, time);
    return glm::mix(scales[idx].first, scales[idx + 1].first, scale);
}

float Bone::Interpolate(float prevTime, float nextTime, float currTime)
{
    auto timePast = std::abs(currTime - prevTime);
    auto timeTotal = std::abs(nextTime - prevTime);
    assert(timeTotal);
    return glm::clamp(timePast / timeTotal, 0.0f, 1.0f);
}

} // namespace RenderIt
