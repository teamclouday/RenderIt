#include "PostProcessMSAA.hpp"

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

} // namespace RenderIt
