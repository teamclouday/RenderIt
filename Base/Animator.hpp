#pragma once
#include <array>
#include <memory>

#include <glm/glm.hpp>

#include "Animation.hpp"
#include "GLStructs.hpp"
#include "Model.hpp"

namespace RenderIt
{

class Animator
{
  public:
    Animator();

    /// Get singleton
    static std::shared_ptr<Animator> Instance();

    /// Update delta time in seconds
    void Update(float deltaSeconds);

    /// Read model animation & prepare bone matrices
    void UpdateAnimation(std::shared_ptr<Model> model);

    /// Bind bone matrices uniform buffer
    void BindBones(unsigned bindingID = 0) const;

    /// UnBind uniform buffer
    void UnBindBones(unsigned bindingID = 0) const;

  public:
    const std::string NAME = "Animator";

  private:
    /// Update uniform buffer
    void updateUniformBuffer();

  private:
    std::array<glm::mat4, ANIMATION_MAX_BONES> _boneMatrices;
    std::unique_ptr<SBuffer> _boneUBO;

    float _deltaT;
};

} // namespace RenderIt
