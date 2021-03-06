/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#include "saiga/imgui/imgui_renderer.h"

#include "saiga/imgui/imgui.h"

namespace Saiga
{
ImGuiRenderer::ImGuiRenderer()
{
    ImGui::CreateContext();
}

ImGuiRenderer::~ImGuiRenderer()
{
    ImGui::DestroyContext();
}



void ImGuiRenderer::endFrame()
{
    ImGui::Render();
}

void ImGuiRenderer::render()
{
    renderDrawLists(ImGui::GetDrawData());
}


}  // namespace Saiga
