#include "Bone.hpp"
#include "Tools.hpp"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <unordered_set>

#include <glm/gtc/matrix_transform.hpp>

namespace RenderIt
{

Bone::Bone(const std::string &boneName, unsigned boneID, const aiNodeAnim *animNode) : name(boneName), ID(boneID)
{
    assert(animNode);

    std::unordered_set<float> duplicates;

    assert(animNode->mNumPositionKeys);
    for (auto i = 0; i < animNode->mNumPositionKeys; i++)
    {
        auto position = animNode->mPositionKeys[i].mValue;
        auto timestamp = static_cast<float>(std::abs(animNode->mPositionKeys[i].mTime));
        // skip duplicates
        if (duplicates.count(timestamp))
            continue;
        duplicates.insert(timestamp);
        positions.push_back({Tools::convertAssimpVector(position), timestamp});
    }
    // sort ascending
    std::sort(positions.begin(), positions.end(),
              [](const std::pair<glm::vec3, float> &v1, const std::pair<glm::vec3, float> &v2) {
                  return v1.second < v2.second;
              });
    duplicates.clear();

    assert(animNode->mNumRotationKeys);
    for (auto i = 0; i < animNode->mNumRotationKeys; i++)
    {
        auto rotation = animNode->mRotationKeys[i].mValue;
        auto timestamp = static_cast<float>(std::abs(animNode->mRotationKeys[i].mTime));
        // skip duplicates
        if (duplicates.count(timestamp))
            continue;
        duplicates.insert(timestamp);
        rotations.push_back({glm::normalize(Tools::convertAssimpQuaternion(rotation)), timestamp});
    }
    // sort ascending
    std::sort(rotations.begin(), rotations.end(),
              [](const std::pair<glm::quat, float> &v1, const std::pair<glm::quat, float> &v2) {
                  return v1.second < v2.second;
              });
    duplicates.clear();

    assert(animNode->mNumScalingKeys);
    for (auto i = 0; i < animNode->mNumScalingKeys; i++)
    {
        auto scale = animNode->mScalingKeys[i].mValue;
        auto timestamp = static_cast<float>(std::abs(animNode->mScalingKeys[i].mTime));
        // skip duplicates
        if (duplicates.count(timestamp))
            continue;
        duplicates.insert(timestamp);
        scales.push_back({Tools::convertAssimpVector(scale), timestamp});
    }
    // sort ascending
    std::sort(scales.begin(), scales.end(),
              [](const std::pair<glm::vec3, float> &v1, const std::pair<glm::vec3, float> &v2) {
                  return v1.second < v2.second;
              });
    duplicates.clear();
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
    if (positions.size() == 1 || time <= positions[0].second)
        return positions[0].first;

    // find target index
    unsigned idx = 0;
    for (; idx < positions.size() - 1; idx++)
    {
        if (time < positions[idx + 1].second)
            break;
    }
    if (idx == positions.size() - 1)
        return positions[idx].first;
    // get interpolated position
    auto scale = Interpolate(positions[idx].second, positions[idx + 1].second, time);
    return glm::mix(positions[idx].first, positions[idx + 1].first, scale);
}

glm::quat Bone::InterpolateRotation(float time)
{
    if (rotations.size() == 1 || time <= rotations[0].second)
        return rotations[0].first;

    // find target index
    unsigned idx = 0;
    for (; idx < rotations.size() - 1; idx++)
    {
        if (time < rotations[idx + 1].second)
            break;
    }
    if (idx == rotations.size() - 1)
        return rotations[idx].first;
    // get interpolated scale
    auto scale = Interpolate(rotations[idx].second, rotations[idx + 1].second, time);
    return glm::slerp(rotations[idx].first, rotations[idx + 1].first, scale);
}

glm::vec3 Bone::InterpolateScale(float time)
{
    if (scales.size() == 1 || time <= scales[0].second)
        return scales[0].first;

    // find target index
    unsigned idx = 0;
    for (; idx < scales.size() - 1; idx++)
    {
        if (time < scales[idx + 1].second)
            break;
    }
    if (idx == scales.size() - 1)
        return scales[idx].first;
    // get interpolated scale
    auto scale = Interpolate(scales[idx].second, scales[idx + 1].second, time);
    return glm::mix(scales[idx].first, scales[idx + 1].first, scale);
}

float Bone::Interpolate(float prevTime, float nextTime, float currTime)
{
    auto timePast = currTime - prevTime;
    auto timeTotal = nextTime - prevTime;
    if (timeTotal <= 0.0f || timePast < 0.0f)
    {
        Tools::display_message("Animation Bone",
                               "Invalid interpolation time (" + std::to_string(prevTime) + "," +
                                   std::to_string(nextTime) + "," + std::to_string(currTime) + ")",
                               Tools::MessageType::WARN);
        return 1.0f;
    }
    return timePast / timeTotal;
}

} // namespace RenderIt
