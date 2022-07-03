#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <assimp/anim.h>
#include <glm/glm.hpp>

#include "Bone.hpp"

#define ANIMATION_MAX_BONES 200

namespace RenderIt
{

struct Animation
{
    struct Node
    {
        glm::mat4 transform;
        std::string name;
        std::vector<std::shared_ptr<Node>> children;
    };

    Animation(const aiAnimation *anim, std::unordered_map<std::string, std::pair<unsigned, glm::mat4>> &infoMap);

    bool HasBone(const std::string &name) const;

    std::shared_ptr<Bone> GetBone(const std::string &name) const;

    float duration;
    float ticksPerSecond;

    float currTime;

    std::string name;

    // map bone name -> bone
    std::unordered_map<std::string, std::shared_ptr<Bone>> bones;
};

} // namespace RenderIt
