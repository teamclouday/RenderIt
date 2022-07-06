#pragma once
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <assimp/anim.h>
#include <glm/glm.hpp>

#include "Bone.hpp"

#define ANIMATION_MAX_BONES 300

/** @file */

namespace RenderIt
{

/// Animation clip
struct Animation
{
    /// Animation structure (scene node structure)
    struct Node
    {
        glm::mat4 transform;
        std::string name;
        std::vector<std::shared_ptr<Node>> children;
    };

    Animation(const aiAnimation *anim,
              std::unordered_map<std::string, std::pair<unsigned, std::optional<glm::mat4>>> &infoMap);

    /// Whether bone with given name exists
    bool HasBone(const std::string &name) const;

    /// Find & get bone with name
    std::shared_ptr<Bone> GetBone(const std::string &name) const;

    float duration;
    float ticksPerSecond;

    float currTime;

    std::string name;

    // map bone name -> bone
    std::unordered_map<std::string, std::shared_ptr<Bone>> bones;
};

} // namespace RenderIt
