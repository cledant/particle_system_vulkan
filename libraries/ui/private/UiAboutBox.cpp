#include "UiAboutBox.hpp"

#include "imgui.h"

#include "AppVersion.hpp"

void
UiAboutBox::draw()
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
    ImGui::Begin("About", &isOpen, WIN_FLAGS);
    ImGui::Text("particle_system");
    ImGui::Separator();
    ImGui::Text("Version: %d.%d.%d",
                particle_sys::APP_VERSION_MAJOR,
                particle_sys::APP_VERSION_MINOR,
                particle_sys::APP_VERSION_PATCH);
    ImGui::Separator();
    ImGui::Text("Commit: %s", particle_sys::APP_COMMIT_HASH);
    ImGui::End();
}
