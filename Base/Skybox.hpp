#pragma once
#include <memory>
#include <string>

#include "GLStructs.hpp"
#include "Shader.hpp"

#include <glm/glm.hpp>

namespace RenderIt
{

class Skybox
{
  public:
    Skybox();

    bool Load(const std::string &panoramaMap);

    bool Load(const std::string &posX, const std::string &negX, const std::string &posY, const std::string &negY,
              const std::string &posZ, const std::string &negZ);

    void ClearBackground(const glm::mat4 &mProj, const glm::mat4 &mView) const;

    GLuint GetSkybox() const;

  public:
    const std::string NAME = "Skybox";

  private:
    void loadDrawShaders();

  private:
    std::unique_ptr<STexture> _skybox;
    std::shared_ptr<Shader> _drawShader;
};

} // namespace RenderIt
