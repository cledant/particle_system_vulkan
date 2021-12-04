#ifndef PARTICLE_SYS_VULKAN_UISIMPLEINPUTWINDOW_HPP
#define PARTICLE_SYS_VULKAN_UISIMPLEINPUTWINDOW_HPP

#include <string>

#include "UiTextBox.hpp"

struct UiSimpleInputWindow final
{
    std::string windowName = "No Name";
    std::string windowText = "No Text";
    uint32_t winW = 500;
    uint32_t winH = 80;
    char input[4096]{};
    bool forceFocus = true;
    bool isInputOpen = false;
    UiTextBox errorWin{};

    bool drawInputWindow();
};

#endif // PARTICLE_SYS_VULKAN_UISIMPLEINPUTWINDOW_HPP
