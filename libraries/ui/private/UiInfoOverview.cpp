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
    static ImVec2 const WIN_SIZE_INFO = ImVec2(470, 55);
    static ImVec2 const WIN_SIZE_FPS = ImVec2(165, 55);
    static ImVec2 const WIN_SIZE_BOTH = ImVec2(470, 90);
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
                if (_avg_fps <= 20.0f) {
                    ImGui::PushStyleColor(ImGuiCol_Text, RED);
                } else if (_avg_fps <= 40.0f) {
                    ImGui::PushStyleColor(ImGuiCol_Text, YELLOW);
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Text, GREEN);
                }
                ImGui::SameLine();
                ImGui::Text("%.1f\n", _current_fps);
                ImGui::PopStyleColor();
                ImGui::Text("Avg FPS: ");
                if (_avg_fps <= 20.0f) {
                    ImGui::PushStyleColor(ImGuiCol_Text, RED);
                } else if (_avg_fps <= 40.0f) {
                    ImGui::PushStyleColor(ImGuiCol_Text, YELLOW);
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Text, GREEN);
                }
                ImGui::SameLine();
                ImGui::Text("%.1f\n", _avg_fps);
                ImGui::PopStyleColor();
            }
            if (fps && info) {
                ImGui::Separator();
            }
            if (info) {
                ImGui::Text("Camera position: X = %.2f | Y = %.2f | Z = %.2f",
                            _camera_pos.x,
                            _camera_pos.y,
                            _camera_pos.z);
                ImGui::Text(
                  "Particles gravity center: X = %.2f | Y = %.2f | Z = %.2f",
                  _gravity_center_pos.x,
                  _gravity_center_pos.y,
                  _gravity_center_pos.z);
            }
            ImGui::End();
        }
    }
}

void
UiInfoOverview::setAvgFps(float avgFps)
{
    _avg_fps = avgFps;
}

void
UiInfoOverview::setCurrentFps(float currentFps)
{
    _current_fps = currentFps;
}

void
UiInfoOverview::setCameraPos(glm::vec3 const &cameraPos)
{
    _camera_pos = cameraPos;
}

void
UiInfoOverview::setGravityCenterPos(glm::vec3 const &gravityCenterPos)
{
    _gravity_center_pos = gravityCenterPos;
}
