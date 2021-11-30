#include "UiInfoOverview.hpp"

#include "imgui.h"

void
UiInfoOverview::draw(bool &fps, bool &info) const
{
    static constexpr float const PADDING = 10.0f;
    static constexpr ImGuiWindowFlags const WIN_FLAGS =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
      ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
    static ImVec2 const WIN_SIZE_INFO = ImVec2(470, 115);
    static ImVec2 const WIN_SIZE_FPS = ImVec2(165, 50);
    static ImVec2 const WIN_SIZE_BOTH = ImVec2(470, 150);
    static ImVec2 const WIN_POS_PIVOT = { 1.0f, 0.0f };
    static constexpr float const WIN_ALPHA = 0.35f;
    static ImVec4 const RED = { 255, 0, 0, 255 };
    static ImVec4 const YELLOW = { 255, 255, 0, 255 };
    static ImVec4 const GREEN = { 0, 255, 0, 255 };

    if (info || fps) {
        ImGuiViewport const *viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos;
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos{ (work_pos.x + work_size.x - PADDING),
                           (work_pos.y + PADDING) };

        if (fps && info) {
            ImGui::SetNextWindowSize(WIN_SIZE_BOTH);
        } else if (fps) {
            ImGui::SetNextWindowSize(WIN_SIZE_FPS);
        } else {
            ImGui::SetNextWindowSize(WIN_SIZE_INFO);
        }
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, WIN_POS_PIVOT);
        ImGui::SetNextWindowBgAlpha(WIN_ALPHA);
        if (ImGui::Begin("Info Overview", nullptr, WIN_FLAGS)) {
            if (fps) {
                ImGui::Text("Current FPS: ");
                if (avgFps <= 20.0f) {
                    ImGui::PushStyleColor(ImGuiCol_Text, RED);
                } else if (avgFps <= 40.0f) {
                    ImGui::PushStyleColor(ImGuiCol_Text, YELLOW);
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Text, GREEN);
                }
                ImGui::SameLine();
                ImGui::Text("%.1f\n", currentFps);
                ImGui::PopStyleColor();
                ImGui::Text("Avg FPS: ");
                if (avgFps <= 20.0f) {
                    ImGui::PushStyleColor(ImGuiCol_Text, RED);
                } else if (avgFps <= 40.0f) {
                    ImGui::PushStyleColor(ImGuiCol_Text, YELLOW);
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Text, GREEN);
                }
                ImGui::SameLine();
                ImGui::Text("%.1f\n", avgFps);
                ImGui::PopStyleColor();
            }
            if (fps && info) {
                ImGui::Separator();
            }
            if (info) {
                ImGui::Text("Camera position: X = %.2f | Y = %.2f | Z = %.2f",
                            cameraPos.x,
                            cameraPos.y,
                            cameraPos.z);
                ImGui::Text(
                  "Particles gravity center: X = %.2f | Y = %.2f | Z = %.2f",
                  gravityCenterPos.x,
                  gravityCenterPos.y,
                  gravityCenterPos.z);
                ImGui::Text("Nb Particles: %u", nbParticles);
                ImGui::Text("Particle max speed: %u", maxSpeedParticle);
                ImGui::Text("Cursor position 2D: W = %.0f | H = %.0f",
                            cursorPositionWindow.x,
                            cursorPositionWindow.y);
                ImGui::Text(
                  "Cursor position 3D: X = %.2f | Y = %.2f | Z = %.2f",
                  cursorPosition3D.x,
                  cursorPosition3D.y,
                  cursorPosition3D.z);
            }
            ImGui::End();
        }
    }
}
