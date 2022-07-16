#include "PostProcess/PostProcessGeneral.hpp"
#include "Tools.hpp"

#include <algorithm>
#include <string>

namespace RenderIt
{

PostProcessGeneral::PostProcessGeneral() : _RBO(nullptr), _TEX(nullptr)
{
    _frameWidth = 1;
    _frameHeight = 1;
    _FBO = nullptr;
    loadShader();
    loadVAO();
}

void PostProcessGeneral::StartRecord()
{
    if (_FBO)
        _FBO->Bind();
}

void PostProcessGeneral::StopRecord()
{
    if (_FBO)
        _FBO->UnBind();
}

void PostProcessGeneral::Draw(std::function<void(const Shader *)> func)
{
    _shader->Bind();
    if (func)
        func(_shader.get());
    if (_TEX)
    {
        _shader->UniformInt("screenTexture", 0);
        _shader->TextureBinding(_TEX->Get(), 0u);
    }
    else
        Tools::display_message(NAME, "no screen texture!", Tools::MessageType::WARN);
    _VAO->Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    _VAO->UnBind();
    _shader->UnBind();
}

bool PostProcessGeneral::Update(int screenWidth, int screenHeight)
{
    if (screenWidth == _frameWidth && screenHeight == _frameHeight && _FBO)
        return true;
    _frameWidth = std::max(1, screenWidth);
    _frameHeight = std::max(1, screenHeight);
    return loadFBO();
}

GLuint PostProcessGeneral::GetTexture() const
{
    return _TEX ? _TEX->Get() : 0u;
}

void PostProcessGeneral::SetShader(std::shared_ptr<Shader> newShader)
{
    if (newShader)
        _shader = newShader;
}

void PostProcessGeneral::loadShader()
{
    std::string vertShader = R"(
        #version 450 core
        layout(location = 0) in vec2 inPos;
        layout(location = 0) out vec2 vertTexCoords;
        void main()
        {
            vertTexCoords = inPos * 0.5 + 0.5;
            gl_Position = vec4(inPos, 0.0, 1.0);
        }
    )";
    std::string fragShader = R"(
        #version 450 core
        layout(location = 0) out vec4 outColor;
        layout(location = 0) in vec2 vertTexCoords;
        uniform sampler2D screenTexture;
        void main()
        {
            outColor = vec4(texture(screenTexture, vertTexCoords).rgb, 1.0);
        }
    )";
    _shader = std::make_unique<Shader>();
    _shader->AddSource(vertShader, GL_VERTEX_SHADER);
    _shader->AddSource(fragShader, GL_FRAGMENT_SHADER);
    _shader->Compile();
}

void PostProcessGeneral::loadVAO()
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

bool PostProcessGeneral::loadFBO()
{
    _TEX = std::make_unique<STexture>(GL_TEXTURE_2D);
    _TEX->Bind();
    glTexImage2D(_TEX->type, 0, GL_RGBA16F, _frameWidth, _frameHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    _TEX->UnBind();

    _RBO = std::make_unique<SRBO>();
    _RBO->Bind();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _frameWidth, _frameHeight);
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
