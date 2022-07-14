#pragma once
#include "RenderIt.hpp"

#include <algorithm>
#include <memory>
#include <string>

#include <imgui.h>

using namespace RenderIt;

struct ScreenRecorder
{
    ScreenRecorder()
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
        VAO = std::make_unique<SVAO>();
        VBO = std::make_unique<SBuffer>(GL_ARRAY_BUFFER);
        VAO->Bind();
        VBO->Bind();
        glBufferData(VBO->type, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        VAO->UnBind();

        shader = std::make_unique<Shader>();
        shader->AddSource(Tools::read_file_content("./shaders/Screen.vert"), GL_VERTEX_SHADER);
        shader->AddSource(Tools::read_file_content("./shaders/Screen.frag"), GL_FRAGMENT_SHADER);
        shader->Compile();
    }

    bool Update(int w, int h)
    {
        if (dimW == w && dimH == h && screenFBO)
            return true;
        // update dimensions
        dimW = std::max(1, w);
        dimH = std::max(1, h);
        // init textures and buffers
        screenTex = std::make_unique<STexture>(GL_TEXTURE_2D);
        screenTex->Bind();
        glTexImage2D(screenTex->type, 0, GL_RGB, dimW, dimH, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        screenTex->UnBind();
        screenRBO = std::make_unique<SRBO>();
        screenRBO->Bind();
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, dimW, dimH);
        screenRBO->UnBind();
        screenFBO = std::make_unique<SFBO>();
        // attach texture
        screenFBO->Bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, screenTex->type, screenTex->Get(), 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, screenRBO->Get());
        screenFBO->UnBind();
        // validate
        if (screenFBO->Validate())
        {
            Tools::display_message("ScreenRecorder", "Framebuffer updated", Tools::MessageType::INFO);
            return true;
        }
        else
        {
            Tools::display_message("ScreenRecorder", "Framebuffer failed to update", Tools::MessageType::WARN);
            return false;
        }
    }

    void Bind()
    {
        if (screenFBO)
            screenFBO->Bind();
    }

    void UnBind()
    {
        if (screenFBO)
            screenFBO->UnBind();
    }

    void Draw()
    {
        shader->Bind();
        if (screenTex)
        {
            shader->UniformInt("screenTexture", 0);
            shader->TextureBinding(screenTex->Get(), 0u);
        }
        VAO->Bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        VAO->UnBind();
        shader->UnBind();
    }

    int dimW = 0, dimH = 0;
    std::unique_ptr<STexture> screenTex;
    std::unique_ptr<SRBO> screenRBO;
    std::unique_ptr<SFBO> screenFBO;

    std::unique_ptr<SVAO> VAO;
    std::unique_ptr<SBuffer> VBO;

    std::unique_ptr<Shader> shader;
};
