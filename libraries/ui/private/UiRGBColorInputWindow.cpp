#include "UiRGBColorInputWindow.hpp"

#include "imgui.h"

bool
UiRGBColorInputWindow::drawInputWindow()
{
    static constexpr ImGuiWindowFlags const WIN_FLAGS =
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    static ImVec2 const WIN_SIZE = ImVec2(winW, winH);
    static ImVec2 const WIN_POS_PIVOT = { 0.5f, 0.5f };
    static constexpr ImGuiInputTextFlags const INPUT_COLOR_FLAGS =
      ImGuiColorEditFlags_Float | ImGuiColorEditFlags_PickerHueBar |
      ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha;

    if (!isInputOpen) {
        return (false);
    }

    ImGuiViewport const *viewport = ImGui::GetMainViewport();
    auto viewport_center = viewport->GetCenter();
    ImVec2 window_pos{ viewport_center.x, viewport_center.y };
    bool trigger = false;
    static ImVec4 work_color = { 0.0f, 0.5f, 0.3f, 0.0f };

    ImGui::SetNextWindowSize(WIN_SIZE);
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, WIN_POS_PIVOT);
    ImGui::Begin(windowName.c_str(), &isInputOpen, WIN_FLAGS);

    ImGui::ColorPicker4(
      "Preview", reinterpret_cast<float *>(&work_color), INPUT_COLOR_FLAGS);
    ImGui::NewLine();
    bool ok_pressed = ImGui::Button("Ok");
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        isInputOpen = false;
    }
    if (ok_pressed) {
        color = work_color;
        trigger = true;
        isInputOpen = false;
    }
    ImGui::End();
    return (trigger);
}