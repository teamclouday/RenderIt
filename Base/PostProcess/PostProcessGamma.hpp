#pragma once
#include "GLStructs.hpp"
#include "PostProcess/PostProcess.hpp"
#include "Shader.hpp"

/** @file */

namespace RenderIt
{

/// Post-Processing for Gamma Correction
class PostProcessGamma : public PostProcess
{
  public:
    PostProcessGamma();

    void StartRecord() override;

    void StopRecord() override;

    void Draw(std::function<void(const Shader *)> func = nullptr) override;

    bool Update(int screenWidth, int screenHeight) override;

    /// Get current gamma
    float GetGamma() const;

    /// Set gamma
    void SetGamma(float val);

    void UI();

  public:
    const std::string NAME = "PostProcessGamma";

  private:
    /// Load shader program
    void loadShader();

    /// Load vertex array
    void loadVAO();

    /// Load framebuffer
    bool loadFBO();

  private:
    float _gamma, _gammaInv;
    std::shared_ptr<Shader> _shader;
    std::unique_ptr<SRBO> _RBO;
    std::unique_ptr<STexture> _TEX;
    std::unique_ptr<SVAO> _VAO;
    std::unique_ptr<SBuffer> _VBO;
};

} // namespace RenderIt
