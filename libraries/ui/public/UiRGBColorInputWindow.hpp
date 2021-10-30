#ifndef PARTICLE_SYS_VULKAN_UIRGBCOLORINPUTWINDOW_HPP
#define PARTICLE_SYS_VULKAN_UIRGBCOLORINPUTWINDOW_HPP

#include <string>

#include "imgui.h"

struct UiRGBColorInputWindow final
{
    std::string windowName = "No Name";
    uint32_t winW = 400;
    uint32_t winH = 0;
    ImVec4 color{};
    bool isInputOpen = false;

    bool drawInputWindow();
};

#endif // PARTICLE_SYS_VULKAN_UIRGBCOLORINPUTWINDOW_HPP
