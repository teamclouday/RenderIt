#pragma once
#include <memory>
#include <vector>

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
    void Bind(unsigned bindingID = 0) const;

    /// UnBind uniform buffer
    void UnBind() const;

  public:
    const std::string NAME = "Animator";

  private:
    /// Update uniform buffer
    void updateUniformBuffer();

  private:
    std::vector<glm::mat4> _boneMatrices;
    std::unique_ptr<SBuffer> _boneUBO;

    float _deltaT;
};

} // namespace RenderIt
