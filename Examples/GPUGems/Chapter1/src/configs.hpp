#pragma once
#include "RenderIt.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include <memory>
#include <string>
#include <vector>

using namespace RenderIt;

#define MAX_WAVE_COUNT 100

enum class SineWaveType
{
    Directional = 0,
    Circular = 1,
};

struct SineWave
{
    SineWave(SineWaveType waveType = SineWaveType::Directional, float waveLength = 0.1f, float amplitude = 0.05f,
             float waveSpeed = 0.1f, float waveSteep = 0.1f, const glm::vec2 &waveDir = glm::vec2(1.0f),
             const glm::vec2 &waveCenter = glm::vec2(0.0f))
    {
        type = static_cast<int>(waveType);
        len = waveLength;
        amp = amplitude;
        speed = waveSpeed;
        steepness = waveSteep;
        dir = waveDir;
        center = waveCenter;
    }

    int type;
    float len;
    float amp;
    float speed;
    float steepness;
    glm::vec2 dir;
    glm::vec2 center;
};

struct WaveConfigs
{
    WaveConfigs()
    {
        tessLevel = 50.0f;
        powerConstant = 1.0f;
        enableGerstnerWave = true;

        transform.scale = glm::vec3(2.0f);
        transform.rotation.x = -90.0f;
        transform.UpdateMatrix();

        color = glm::vec3(0.8f, 1.0f, 1.0f);

        waves.reserve(MAX_WAVE_COUNT);
        waves.push_back(SineWave());

        wavesBuffer = std::make_unique<SBuffer>(GL_SHADER_STORAGE_BUFFER);
        wavesBuffer->Bind();
        glBufferData(wavesBuffer->type, MAX_WAVE_COUNT * sizeof(SineWave) + sizeof(unsigned), nullptr, GL_DYNAMIC_DRAW);
        wavesBuffer->UnBind();
        UpdateSSBO();
    }

    void UI()
    {
        ImGui::Checkbox("Enable Gerstner Wave", &enableGerstnerWave);
        ImGui::DragFloat("Tess Level", &tessLevel, 1.0f, 1.0f, 200.0f, "%.0f");
        if (!enableGerstnerWave)
            ImGui::DragFloat("Power", &powerConstant, 0.001f, 1.0f, 10.0f, "%.3f");
        ImGui::Separator();
        bool updateT = false;
        updateT |= ImGui::DragFloat3("Position", glm::value_ptr(transform.position), 0.01f);
        updateT |= ImGui::DragFloat3("Rotation", glm::value_ptr(transform.rotation), 0.01f);
        updateT |= ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), 0.01f);
        if (transform.scale.x == transform.scale.y && transform.scale.x == transform.scale.z)
        {
            if (ImGui::DragFloat("Scale All", &transform.scale.x, 0.01f))
            {
                transform.scale.y = transform.scale.x;
                transform.scale.z = transform.scale.x;
                updateT = true;
            }
        }
        if (updateT)
            transform.UpdateMatrix();
        ImGui::Separator();
        ImGui::ColorEdit3("Surface Color", glm::value_ptr(color));
        ImGui::Separator();
        ImGui::Text("Waves");
        auto updateSSBO = false;
        for (auto i = 0; i < waves.size(); i++)
        {
            ImGui::PushID(i);
            if (ImGui::TreeNode(("Wave " + std::to_string(i)).c_str()))
            {
                auto &w = waves[i];
                if (ImGui::TreeNode("Type"))
                {
                    updateSSBO |=
                        ImGui::RadioButton("Directional", &w.type, static_cast<int>(SineWaveType::Directional));
                    updateSSBO |= ImGui::RadioButton("Circular", &w.type, static_cast<int>(SineWaveType::Circular));
                    ImGui::TreePop();
                }
                updateSSBO |= ImGui::DragFloat("Length", &w.len, 0.001f, 0.0f, 1.0f, "%.3f");
                updateSSBO |= ImGui::DragFloat("Amplitude", &w.amp, 0.001f, 0.0f, 1.0f, "%.3f");
                updateSSBO |= ImGui::DragFloat("Speed", &w.speed, 0.001f, 0.0f, 1.0f, "%.3f");
                updateSSBO |= enableGerstnerWave
                                  ? ImGui::DragFloat("Steepness", &w.steepness, 0.001f, 0.0f, 1.0f, "%.3f")
                                  : false;
                updateSSBO |= ImGui::DragFloat2("Direction", glm::value_ptr(w.dir), 0.001f, -1.0f, 1.0f, "%.3f");
                if (w.type == static_cast<int>(SineWaveType::Circular))
                    updateSSBO |= ImGui::DragFloat2("Center", glm::value_ptr(w.center), 0.001f, -1.0f, 1.0f, "%.3f");
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        if (ImGui::Button("Push") && waves.size() < MAX_WAVE_COUNT)
        {
            waves.push_back(SineWave());
            updateSSBO = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Pop") && !waves.empty())
        {
            waves.pop_back();
            updateSSBO = true;
        }
        if (updateSSBO)
            UpdateSSBO();
    }

    void UpdateSSBO()
    {
        wavesBuffer->Bind();
        auto sizePtr =
            reinterpret_cast<unsigned *>(glMapBufferRange(wavesBuffer->type, 0, sizeof(unsigned), GL_MAP_WRITE_BIT));
        *sizePtr = static_cast<unsigned>(waves.size());
        glUnmapBuffer(wavesBuffer->type);
        if (!waves.empty())
        {
            auto dataPtr = glMapBufferRange(wavesBuffer->type, sizeof(unsigned), waves.size() * sizeof(SineWave),
                                            GL_MAP_WRITE_BIT);
            std::memcpy(dataPtr, waves.data(), waves.size() * sizeof(SineWave));
            glUnmapBuffer(wavesBuffer->type);
        }
        wavesBuffer->UnBind();
    }

    float tessLevel;
    float powerConstant;

    bool enableGerstnerWave;

    Transform transform;
    glm::vec3 color;

    std::vector<SineWave> waves;
    std::unique_ptr<SBuffer> wavesBuffer;
};
