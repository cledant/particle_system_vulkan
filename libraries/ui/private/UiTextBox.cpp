#include "UiTextBox.hpp"

#include "imgui.h"

void
UiTextBox::draw()
{
    static constexpr ImGuiWindowFlags const WIN_FLAGS =
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoMove;
    static ImVec2 const WIN_POS_PIVOT = { 0.5f, 0.5f };

    if (!isOpen) {
        return;
    }

    ImGuiViewport const *viewport = ImGui::GetMainViewport();
    auto viewport_center = viewport->GetCenter();
    ImVec2 window_pos{ viewport_center.x, viewport_center.y };

    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, WIN_POS_PIVOT);
    ImGui::Begin(windowName.c_str(), &isOpen, WIN_FLAGS);
    ImGui::Text("%s", windowText.c_str());
    ImGui::End();
}
