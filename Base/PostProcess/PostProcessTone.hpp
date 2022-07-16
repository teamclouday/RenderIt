#pragma once
#include "GLStructs.hpp"
#include "PostProcess/PostProcess.hpp"
#include "Shader.hpp"

/** @file */

namespace RenderIt
{

/// Post-Processing Filter for Tone Mapping (Reinhard)
class PostProcessTone : public PostProcess
{
  public:
    PostProcessTone();

    void StartRecord() override;

    void StopRecord() override;

    void Draw(std::function<void(const Shader *)> func = nullptr) override;

    bool Update(int screenWidth, int screenHeight) override;

    /// Adjust exposure by average luminance
    void AdjustExposure(float averageLum);

    void UI();

  public:
    const std::string NAME = "PostProcessTone";

  private:
    /// Load shader program
    void loadShader();

    /// Load vertex array
    void loadVAO();

    /// Load framebuffer
    bool loadFBO();

  private:
    float _exposure;
    std::shared_ptr<Shader> _shader;
    std::unique_ptr<SRBO> _RBO;
    std::unique_ptr<STexture> _TEX;
    std::unique_ptr<SVAO> _VAO;
    std::unique_ptr<SBuffer> _VBO;
};

} // namespace RenderIt
