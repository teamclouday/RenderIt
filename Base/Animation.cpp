#include "Animation.hpp"

#include <cassert>

namespace RenderIt
{

Animation::Animation(const aiAnimation *anim, std::unordered_map<std::string, std::pair<unsigned, glm::mat4>> &infoMap)
{
    assert(anim);
    duration = anim->mDuration;
    ticksPerSecond = !anim->mTicksPerSecond ? 1.0f : static_cast<float>(anim->mTicksPerSecond);
    currTime = 0.0f;
    name = anim->mName.C_Str();
    assert(duration > 0.0f);
    // collect bones
    for (auto channelIdx = 0; channelIdx < anim->mNumChannels; ++channelIdx)
    {
        auto channel = anim->mChannels[channelIdx];
        std::string boneName = channel->mNodeName.C_Str();
        if (!infoMap.count(boneName))
        {
            unsigned boneID = infoMap.size();
            infoMap[boneName] = {boneID, glm::mat4(1.0f)};
        }
        // assert no overwrite
        assert(!bones.count(boneName));
        bones[boneName] = std::make_shared<Bone>(boneName, infoMap[boneName].first, channel);
    }
    assert(bones.size() < ANIMATION_MAX_BONES);
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
