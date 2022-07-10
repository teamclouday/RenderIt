#pragma once
#include "RenderIt.hpp"

#include <algorithm>
#include <memory>
#include <string>

#include <imgui.h>

using namespace RenderIt;

struct MSAAFramebuffer
{
    MSAAFramebuffer()
    {
        // init draw arrays
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

        // init shader
        shader = std::make_unique<Shader>();
        shader->AddSource(Tools::read_file_content("./shaders/MSAA.vert"), GL_VERTEX_SHADER);
        shader->AddSource(Tools::read_file_content("./shaders/MSAA.frag"), GL_FRAGMENT_SHADER);
        shader->Compile();
    }

    bool Update(int w, int h, int samples)
    {
        if (texW == w && texH == h && nSamples == samples && fboMSAA)
            return true;
        // update dimensions
        texW = std::max(1, w);
        texH = std::max(1, h);
        nSamples = std::max(1, samples);
        // init textures and buffers
        texMSAA = std::make_unique<STexture>(GL_TEXTURE_2D_MULTISAMPLE);
        texMSAA->Bind();
        glTexImage2DMultisample(texMSAA->type, nSamples, GL_RGB, texW, texH, GL_TRUE);
        texMSAA->UnBind();
        rboMSAA = std::make_unique<SRBO>();
        rboMSAA->Bind();
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, nSamples, GL_DEPTH24_STENCIL8, texW, texH);
        rboMSAA->UnBind();
        fboMSAA = std::make_unique<SFBO>();
        // attach texture
        fboMSAA->Bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texMSAA->type, texMSAA->Get(), 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboMSAA->Get());
        fboMSAA->UnBind();
        // validate
        if (fboMSAA->Validate())
        {
            Tools::display_message("MSAA", "Framebuffer updated", Tools::MessageType::INFO);
            return true;
        }
        else
        {
            Tools::display_message("MSAA", "Framebuffer failed to update", Tools::MessageType::WARN);
            return false;
        }
    }

    void Bind()
    {
        if (fboMSAA)
            fboMSAA->Bind();
    }

    void UnBind()
    {
        if (fboMSAA)
            fboMSAA->UnBind();
    }

    void Draw()
    {
        shader->Bind();
        if (texMSAA)
        {
            shader->UniformInt("screenTexture", 0);
            glActiveTexture(GL_TEXTURE0);
            texMSAA->Bind();
        }
        shader->UniformInt("numSamples", nSamples);
        VAO->Bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        VAO->UnBind();
        shader->UnBind();
    }

    int texW = 0, texH = 0, nSamples = 0;
    std::unique_ptr<STexture> texMSAA;
    std::unique_ptr<SRBO> rboMSAA;
    std::unique_ptr<SFBO> fboMSAA;

    std::unique_ptr<SVAO> VAO;
    std::unique_ptr<SBuffer> VBO;

    std::unique_ptr<Shader> shader;
};
