#pragma once
#include "GLStructs.hpp"
#include "PostProcess/PostProcess.hpp"
#include "Shader.hpp"

/** @file */

namespace RenderIt
{

/// General Post-Processing Filter
class PostProcessGeneral : public PostProcess
{
  public:
    PostProcessGeneral();

    void StartRecord() override;

    void StopRecord() override;

    void Draw(std::function<void(const Shader *)> func = nullptr) override;

    bool Update(int screenWidth, int screenHeight) override;

    /// Get color texture of framebuffer
    GLuint GetTexture() const;

    /// Override default shader
    void SetShader(std::shared_ptr<Shader> newShader);

  public:
    const std::string NAME = "PostProcessGeneral";

  private:
    /// Load shader program
    void loadShader();

    /// Load vertex array
    void loadVAO();

    /// Load framebuffer
    bool loadFBO();

  private:
    std::shared_ptr<Shader> _shader;
    std::unique_ptr<SRBO> _RBO;
    std::unique_ptr<STexture> _TEX;
    std::unique_ptr<SVAO> _VAO;
    std::unique_ptr<SBuffer> _VBO;
};

} // namespace RenderIt
