#pragma once
#include "GLStructs.hpp"
#include "PostProcess/PostProcess.hpp"
#include "Shader.hpp"

/** @file */

namespace RenderIt
{

/// Post-Processing for Multi-Sample Anti-Aliasing
class PostProcessMSAA : public PostProcess
{
  public:
    PostProcessMSAA();

    void StartRecord() override;

    void StopRecord() override;

    void Draw(std::function<void(const Shader *)> func = nullptr) override;

    bool Update(int screenWidth, int screenHeight) override;

    /// Get number of samples
    int GetNumSamples() const;

    /// Set number of samples
    bool SetNumSamples(int samples);

    void UI();

  public:
    const std::string NAME = "PostProcessMSAA";

  private:
    /// Load shader program
    void loadShader();

    /// Load vertex array
    void loadVAO();

    /// Load framebuffer
    bool loadFBO();

  private:
    int _numSamples;
    std::shared_ptr<Shader> _shader;
    std::unique_ptr<SRBO> _RBO;
    std::unique_ptr<STexture> _TEX;
    std::unique_ptr<SVAO> _VAO;
    std::unique_ptr<SBuffer> _VBO;
};

} // namespace RenderIt
