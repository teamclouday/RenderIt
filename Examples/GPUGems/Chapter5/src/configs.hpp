#pragma once
#include "RenderIt.hpp"

#include <cmath>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

using namespace RenderIt;

struct NoiseConfigs
{
    void UI()
    {
        ImGui::DragFloat("Tess Level", &tessLevel, 1.0f, 1.0f, 100.0f, "%.0f");
        ImGui::DragFloat("Resolution", &controlRes, 0.1f, 0.0f, 10000.0f, "%.2f");
        ImGui::DragFloat("Control", &controlTime, 0.001f, 0.0f, 1.0f, "%.3f");
        ImGui::Checkbox("Use Time", &controlByTime);
        ImGui::ColorEdit3("Color", glm::value_ptr(baseColor));
        ImGui::Separator();
        ImGui::Text("Type");
        ImGui::RadioButton("LUMPY", &controlType, 0);
        ImGui::RadioButton("MARBLED", &controlType, 1);
        ImGui::RadioButton("CRINKLED", &controlType, 2);
    }

    void Update(float time)
    {
        if (controlByTime)
            controlTime = std::fmod(time, 1.0f);
    }

    float tessLevel = 5.0f;
    int controlType = 0;
    float controlRes = 100.0f;
    float controlTime = 0.0f;
    bool controlByTime = false;
    glm::vec3 baseColor = glm::vec3(1.0f);
};
