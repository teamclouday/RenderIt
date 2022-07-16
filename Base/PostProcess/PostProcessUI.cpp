#include "PostProcess/PostProcessGamma.hpp"
#include "PostProcess/PostProcessMSAA.hpp"

#include <imgui.h>

namespace RenderIt
{

void PostProcess::UI()
{
    ImGui::Text("Frame Size: (%dx%d)", _frameWidth, _frameHeight);
}

void PostProcessMSAA::UI()
{
    PostProcess::UI();
    if (ImGui::DragInt("Num Samples", &_numSamples, 1, 1, 8))
    {
        loadFBO();
    }
}

void PostProcessGamma::UI()
{
    PostProcess::UI();
    if (ImGui::DragFloat("Gamma", &_gamma, 0.01f, 0.01f, 10.0f, "%.2f"))
    {
        _gammaInv = 1.0f / _gamma;
    }
}

} // namespace RenderIt
