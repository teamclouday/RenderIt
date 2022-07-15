#include "PostProcess/PostProcessMSAA.hpp"

#include <algorithm>
#include <string>

namespace RenderIt
{

PostProcessMSAA::PostProcessMSAA() : _numSamples(4), _RBO(nullptr), _TEX(nullptr)
{
    _frameWidth = 1;
    _frameHeight = 1;
    _FBO = nullptr;
    loadShader();
    loadVAO();
}

void PostProcessMSAA::StartRecord()
{
    if (_FBO)
        _FBO->Bind();
}

void PostProcessMSAA::StopRecord()
{
    if (_FBO)
        _FBO->UnBind();
}

void PostProcessMSAA::Draw()
{
    _shader->Bind();
    if (_TEX)
    {
        _shader->UniformInt("screenTexture", 0);
        _shader->TextureBinding(_TEX->Get(), 0u);
    }
    _shader->UniformInt("numSamples", _numSamples);
    _VAO->Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    _VAO->UnBind();
    _shader->UnBind();
}

bool PostProcessMSAA::Update(int screenWidth, int screenHeight)
{
    if (screenWidth == _frameWidth && screenHeight == _frameHeight && _FBO)
        return true;
    _frameWidth = std::max(1, screenWidth);
    _frameHeight = std::max(1, screenHeight);
    return loadFBO();
}

int PostProcessMSAA::GetNumSamples() const
{
    return _numSamples;
}

bool PostProcessMSAA::SetNumSamples(int samples)
{
    auto n = std::min(8, std::max(1, samples));
    if (n != _numSamples)
    {
        _numSamples = n;
        return loadFBO();
    }
    return true;
}

void PostProcessMSAA::loadShader()
{
    std::string vertShader = R"(
        #version 450 core
        layout(location = 0) in vec2 inPos;
        void main(){ gl_Position = vec4(inPos, 0.0, 1.0); }
    )";
    std::string fragShader = R"(
        #version 450 core
        layout(location = 0) out vec4 outColor;
        uniform sampler2DMS screenTexture;
        uniform int numSamples;
        void main()
        {
            ivec2 texCoords = ivec2(gl_FragCoord.xy);
            vec3 color = vec3(0.0);
            for (int i = 0; i < numSamples; ++i)
            {
                color += texelFetch(screenTexture, texCoords, i).rgb;
            }
            color /= numSamples;
            outColor = vec4(color, 1.0);
        }
    )";
    _shader = std::make_unique<Shader>();
    _shader->AddSource(vertShader, GL_VERTEX_SHADER);
    _shader->AddSource(fragShader, GL_FRAGMENT_SHADER);
    _shader->Compile();
}

void PostProcessMSAA::loadVAO()
{
    // clang-format off
    const float vertices[] =
    {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
    };
    // clang-format on
    _VAO = std::make_unique<SVAO>();
    _VBO = std::make_unique<SBuffer>(GL_ARRAY_BUFFER);
    _VAO->Bind();
    _VBO->Bind();
    glBufferData(_VBO->type, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    _VAO->UnBind();
}

bool PostProcessMSAA::loadFBO()
{
    _TEX = std::make_unique<STexture>(GL_TEXTURE_2D_MULTISAMPLE);
    _TEX->Bind();
    glTexImage2DMultisample(_TEX->type, _numSamples, GL_RGB, _frameWidth, _frameHeight, GL_TRUE);
    _TEX->UnBind();

    _RBO = std::make_unique<SRBO>();
    _RBO->Bind();
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, _numSamples, GL_DEPTH24_STENCIL8, _frameWidth, _frameHeight);
    _RBO->UnBind();
    _FBO = std::make_unique<SFBO>();

    _FBO->Bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _TEX->type, _TEX->Get(), 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _RBO->Get());
    _FBO->UnBind();

    if (!_FBO->Validate())
    {
        _FBO = nullptr;
        return false;
    }
    return true;
}

} // namespace RenderIt
