#pragma once
#include "RenderIt.hpp"

#include <memory>
#include <string>

using namespace RenderIt;

struct WaveRenderer
{
    WaveRenderer()
    {
        shader = std::make_shared<Shader>();
        shader->AddSource(Tools::read_file_content("./shaders/waves/wave.vert"), GL_VERTEX_SHADER);
        shader->AddSource(Tools::read_file_content("./shaders/waves/wave.tesc"), GL_TESS_CONTROL_SHADER);
        shader->AddSource(Tools::read_file_content("./shaders/waves/wave.tese"), GL_TESS_EVALUATION_SHADER);
        shader->AddSource(Tools::read_file_content("./shaders/waves/wave.geom"), GL_GEOMETRY_SHADER);
        shader->AddSource(Tools::read_file_content("./shaders/waves/wave.frag"), GL_FRAGMENT_SHADER);
        shader->Compile();

        // clang-format off
        const float vertices[] = {
            -1.0f, -1.0f,
             1.0f, -1.0f,
            -1.0f,  1.0f,
            -1.0f,  1.0f,
             1.0f, -1.0f,
             1.0f,  1.0f
        };
        // clang-format on
        VAO = std::make_unique<SVAO>();
        VBO = std::make_unique<SBuffer>(GL_ARRAY_BUFFER);
        VAO->Bind();
        VBO->Bind();
        glBufferData(VBO->type, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
        VAO->UnBind();
    }

    inline void Draw()
    {
        VAO->Bind();
        glDrawArrays(GL_PATCHES, 0, 6);
        VAO->UnBind();
    }

    std::shared_ptr<Shader> shader;
    std::unique_ptr<SVAO> VAO;
    std::unique_ptr<SBuffer> VBO;
};
