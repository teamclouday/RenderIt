#pragma once
#include <array>
#include <memory>

#include <glm/glm.hpp>

#include "Animation.hpp"
#include "GLStructs.hpp"
#include "Model.hpp"

/** @file */

namespace RenderIt
{

/// Animator that updates bones
class Animator
{
  public:
    Animator();

    /// Get singleton
    static std::shared_ptr<Animator> Instance();

    /// Update delta time in seconds
    void Update(float deltaSeconds);

    /// Read model animation & prepare bone matrices
    void UpdateAnimation(const Model *model);

    /// Bind bone matrices uniform buffer
    void BindBones(unsigned bindingID = 0) const;

    /// UnBind uniform buffer
    void UnBindBones(unsigned bindingID = 0) const;

    /// Get reference to bone matrices
    std::array<glm::mat4, ANIMATION_MAX_BONES> &AccessBoneMatrices();

  public:
    const std::string LOGNAME = "Animator";

  private:
    /// Update shader storage buffer
    void updateSSBO(unsigned size = ANIMATION_MAX_BONES);

  private:
    std::array<glm::mat4, ANIMATION_MAX_BONES> _boneMatrices;
    std::unique_ptr<SBuffer> _boneSSBO;

    float _deltaT;
};

} // namespace RenderIt
