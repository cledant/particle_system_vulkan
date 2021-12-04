#ifndef PARTICLE_SYSTEM_VULKAN_UITEXTBOX_HPP
#define PARTICLE_SYSTEM_VULKAN_UITEXTBOX_HPP

#include <string>

struct UiTextBox final
{
    bool isOpen = false;
    std::string windowName = "No Name";
    std::string windowText = "No Text";

    void draw();
};

#endif // PARTICLE_SYSTEM_VULKAN_UITEXTBOX_HPP
