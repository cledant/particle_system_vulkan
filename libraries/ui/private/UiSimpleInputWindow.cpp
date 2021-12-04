#include "UiSimpleInputWindow.hpp"

#include "imgui.h"

bool
UiSimpleInputWindow::drawInputWindow()
{
    static constexpr ImGuiWindowFlags const WIN_FLAGS =
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;
    static ImVec2 const WIN_SIZE = ImVec2(winW, winH);
    static ImVec2 const WIN_POS_PIVOT = { 0.5f, 0.5f };
    static constexpr ImGuiInputTextFlags const INPUT_TEXT_FLAGS =
      ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_NoUndoRedo;

    if (!isInputOpen) {
        return (false);
    }
    errorWin.isOpen = false;

    ImGuiViewport const *viewport = ImGui::GetMainViewport();
    auto viewport_center = viewport->GetCenter();
    ImVec2 window_pos{ viewport_center.x, viewport_center.y };
    bool trigger = false;

    ImGui::SetNextWindowSize(WIN_SIZE);
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, WIN_POS_PIVOT);
    ImGui::Begin(windowName.c_str(), &isInputOpen, WIN_FLAGS);

    bool keyboard_input_ended = ImGui::InputText(
      windowText.c_str(), input, IM_ARRAYSIZE(input), INPUT_TEXT_FLAGS);
    if (forceFocus) {
        ImGui::SetKeyboardFocusHere(0);
        forceFocus = false;
    }
    if (ImGui::IsItemDeactivated() &&
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape))) {
        isInputOpen = false;
        forceFocus = true;
    }
    bool ok_pressed = ImGui::Button("Ok");
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        isInputOpen = false;
        forceFocus = true;
    }
    if (keyboard_input_ended || ok_pressed) {
        trigger = true;
        isInputOpen = false;
        forceFocus = true;
    }
    ImGui::End();
    return (trigger);
}
