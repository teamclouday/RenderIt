#include "Animation.hpp"
#include "Tools.hpp"

#include <algorithm>
#include <string>

namespace RenderIt
{

Animation::Animation(const aiAnimation *anim,
                     std::unordered_map<std::string, std::pair<unsigned, std::optional<glm::mat4>>> &infoMap)
{
    const std::string NAME = "Animation";
    if (!anim)
    {
        Tools::display_message(NAME, "invalid animation data", Tools::MessageType::WARN);
        return;
    }
    duration = static_cast<float>(anim->mDuration);
    ticksPerSecond = !anim->mTicksPerSecond ? 1.0f : static_cast<float>(anim->mTicksPerSecond);
    currTime = 0.0f;
    name = anim->mName.C_Str();
    if (duration <= 0.0f)
        Tools::display_message(NAME, "invalid duration (" + std::to_string(duration) + ")", Tools::MessageType::WARN);
    // collect bones
    for (auto channelIdx = 0u; channelIdx < anim->mNumChannels; ++channelIdx)
    {
        auto channel = anim->mChannels[channelIdx];
        std::string boneName = channel->mNodeName.C_Str();
        if (!infoMap.count(boneName))
        {
            unsigned boneID = static_cast<unsigned>(infoMap.size());
            infoMap[boneName] = {boneID, std::nullopt};
        }
        if (bones.count(boneName))
            Tools::display_message(NAME, "duplicate bone names (" + boneName + ")", Tools::MessageType::WARN);
        bones[boneName] = std::make_shared<Bone>(boneName, infoMap[boneName].first, channel);
    }
    if (bones.size() >= ANIMATION_MAX_BONES)
        Tools::display_message(NAME,
                               "number of bones (" + std::to_string(bones.size()) + ") exceeds limit (" +
                                   std::to_string(ANIMATION_MAX_BONES) + ")",
                               Tools::MessageType::WARN);
}

bool Animation::HasBone(const std::string &name) const
{
    return bones.count(name) > 0;
}

std::shared_ptr<Bone> Animation::GetBone(const std::string &name) const
{
    return HasBone(name) ? bones.at(name) : nullptr;
}

} // namespace RenderIt
