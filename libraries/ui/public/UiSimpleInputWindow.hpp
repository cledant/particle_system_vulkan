#ifndef SCOP_VULKAN_UISIMPLEINPUTWINDOW_HPP
#define SCOP_VULKAN_UISIMPLEINPUTWINDOW_HPP

#include <string>

struct UiSimpleInputWindow final
{
    std::string windowName = "No Name";
    std::string windowText = "No Text";
    std::string errorText = "No Error";
    uint32_t winW = 500;
    uint32_t winH = 80;
    char input[4096] = { 0 };
    bool forceFocus = true;
    bool isInputOpen = false;
    bool isErrorOpen = false;

    bool drawInputWindow();
    void drawInputErrorWindow();
};

#endif // SCOP_VULKAN_UISIMPLEINPUTWINDOW_HPP
