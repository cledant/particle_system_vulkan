#include "UiAboutBox.hpp"

#include "imgui.h"

#include "AppInfo.hpp"

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
    ImGui::Text("%s", app_info::APP_NAME);
    ImGui::Separator();
    ImGui::Text("Version: %d.%d.%d",
                app_info::APP_VERSION_MAJOR,
                app_info::APP_VERSION_MINOR,
                app_info::APP_VERSION_PATCH);
    ImGui::Separator();
    ImGui::Text("Commit: %s", app_info::APP_COMMIT_HASH);
    ImGui::End();
}
