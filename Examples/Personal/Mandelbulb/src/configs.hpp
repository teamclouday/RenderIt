#pragma once
#include "RenderIt.hpp"

#include <imgui.h>

using namespace RenderIt;

struct MandelbulbConfigs
{
    float marchMaxSteps = 40.0f;
    float marchMaxDist = 200.0f;

    int mandelbulbSteps = 4;
    float mandelbulbMaxDist = 2.0f;

    inline void UI()
    {
        ImGui::PushID("Ray Marching");
        ImGui::Text("Ray Marching");
        ImGui::DragFloat("Max Steps", &marchMaxSteps, 1.0f, 0.0f, 100.0f, "%.0f");
        ImGui::DragFloat("Max Dist", &marchMaxDist, 0.1f, 0.1f, 1000.0f, "%.1f");
        ImGui::PopID();
        ImGui::Separator();
        ImGui::PushID("Mandelbulb");
        ImGui::Text("Mandelbulb");
        ImGui::DragInt("Steps", &mandelbulbSteps, 1, 0, 100);
        ImGui::DragFloat("Max Dist", &mandelbulbMaxDist, 0.001f, 0.0f, 10.0f, "%.3f");
        ImGui::PopID();
    }
};
