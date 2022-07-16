#include "PostProcess/PostProcessGamma.hpp"
#include "PostProcess/PostProcessLuminance.hpp"
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

void PostProcessLuminance::UI()
{
    PostProcess::UI();
    ImGui::Text("Max: %.4f", _lumMax);
    ImGui::Text("Min: %.4f", _lumMin);
    ImGui::Text("Avg: %.4f", _lumAvg);
    ImGui::Separator();
    auto updated = false;
    updated &= ImGui::DragFloat("Min Luminance (log)", &_minLog, 0.01f, -1000.0f, _maxLog, "%.2f");
    updated &= ImGui::DragFloat("Max Luminance (log)", &_maxLog, 0.01f, _minLog, 1000.0f, "%.2f");
    if (updated)
    {
        _rangeLog = _maxLog - _minLog;
        _rangeLogInv = _rangeLog ? 1.0f / _rangeLog : 0.0f;
    }
}

} // namespace RenderIt
