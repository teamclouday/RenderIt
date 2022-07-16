#pragma once
#include "GLStructs.hpp"
#include "PostProcess/PostProcess.hpp"
#include "Shader.hpp"

/** @file */

namespace RenderIt
{

/// Post-Processing Filter for Luminance Info
class PostProcessLuminance : public PostProcess
{
  public:
    PostProcessLuminance();

    void StartRecord() override;

    void StopRecord() override;

    bool Update(int screenWidth, int screenHeight) override;

    void Draw(std::function<void(const Shader *)> func = nullptr) override;

    /// Compute to update luminance info
    void Compute();

    /// Get max luminance
    float GetMaxLuminance() const;

    /// Get min luminance
    float GetMinLuminance() const;

    /// Get avg luminance
    float GetAvgLuminance() const;

    void UI();

  public:
    const std::string NAME = "PostProcessLuminance";

  private:
    /// Load shader program
    void loadShader();

    /// Load framebuffer
    bool loadFBO();

    /// Load SSBO
    void loadSSBO();

  private:
    float _lumMax, _lumMin, _lumAvg;
    float _minLog, _maxLog, _rangeLog, _rangeLogInv;
    unsigned _numPixels;
    std::shared_ptr<Shader> _shaderFill, _shaderComp;
    std::unique_ptr<SRBO> _RBO;
    std::unique_ptr<STexture> _TEX;
    std::unique_ptr<SBuffer> _histSSBO, _valsSSBO;
    GLuint _dispatchX, _dispatchY;
};

} // namespace RenderIt
