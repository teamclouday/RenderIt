#include "Animator.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <cassert>
#include <cmath>
#include <queue>
#include <utility>

namespace RenderIt
{
Animator::Animator()
{
    for (auto i = 0; i < ANIMATION_MAX_BONES; ++i)
        _boneMatrices[i] = glm::mat4(1.0f);

    _boneSSBO = std::make_unique<SBuffer>(GL_SHADER_STORAGE_BUFFER);
    _boneSSBO->Bind();
    glBufferData(_boneSSBO->type, ANIMATION_MAX_BONES * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    _boneSSBO->UnBind();
    updateSSBO();

    _deltaT = 0.0f;
}

std::shared_ptr<Animator> Animator::Instance()
{
    static auto animator = std::make_shared<Animator>();
    return animator;
}

void Animator::Update(float deltaSeconds)
{
    _deltaT = deltaSeconds;
}

void Animator::UpdateAnimation(std::shared_ptr<Model> model)
{
    if (!model->HasAnimation() || !_deltaT)
        return;
    auto anim = model->_animations[model->_animationActive];
    assert(anim);
    // update animation time
    anim->currTime += anim->ticksPerSecond * _deltaT;
    anim->currTime = std::fmod(anim->currTime, anim->duration);
    // compute bone transforms
    std::queue<std::pair<std::shared_ptr<Animation::Node>, glm::mat4>> nodes;
    nodes.push({model->_animNodeRoot, glm::mat4(1.0f)});
    unsigned maxMatIdx{0};
    while (!nodes.empty())
    {
        // unpack data
        auto data = nodes.front();
        nodes.pop();
        auto node = data.first;
        auto parentT = data.second;
        // update bone
        auto boneName = node->name;
        auto nodeT = node->transform;
        auto bone = anim->GetBone(boneName);
        if (bone)
        {
            bone->Update(anim->currTime);
            nodeT = bone->matrix;
        }
        // compute current global transform
        auto currT = parentT * nodeT;
        if (model->_boneInfo.count(boneName))
        {
            auto matIdx = model->_boneInfo[boneName].first;
            auto matOffset = model->_boneInfo[boneName].second;
            _boneMatrices[matIdx] = currT * matOffset;
            maxMatIdx = std::max(maxMatIdx, matIdx);
        }
        // recurse
        for (auto child : node->children)
            nodes.push({child, currT});
    }
    updateSSBO(maxMatIdx + 1);
}

void Animator::BindBones(unsigned bindingID) const
{
    _boneSSBO->BindBase(bindingID);
}

void Animator::UnBindBones(unsigned bindingID) const
{
    _boneSSBO->UnBindBase(bindingID);
}

void Animator::updateSSBO(unsigned size)
{
    _boneSSBO->Bind();
    glBufferSubData(_boneSSBO->type, 0, size * sizeof(glm::mat4), _boneMatrices.data());
    _boneSSBO->UnBind();
}
} // namespace RenderIt
