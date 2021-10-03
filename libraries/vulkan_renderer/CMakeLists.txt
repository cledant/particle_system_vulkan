cmake_minimum_required(VERSION 3.17)
project(lib_vulkan_renderer)

add_library(vulkan_renderer STATIC
        private/VulkanRenderer.cpp
        private/VulkanDebug.cpp
        private/VulkanInstance.cpp
        private/VulkanSwapChain.cpp
        private/VulkanSync.cpp
        private/tex/VulkanTextureManager.cpp
        private/ui/VulkanUiRenderPass.cpp
        private/ui/VulkanUi.cpp
        private/utils/VulkanPhysicalDevice.cpp
        private/utils/VulkanSwapChainUtils.cpp
        private/utils/VulkanShader.cpp
        private/utils/VulkanImage.cpp
        private/utils/VulkanMemory.cpp
        private/utils/VulkanCommandBuffer.cpp)
target_include_directories(vulkan_renderer
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/public
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)
set_target_properties(vulkan_renderer PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)
add_dependencies(vulkan_renderer
        fmt
        imgui_glfw_vulkan)
target_link_libraries(vulkan_renderer
        glm
        fmt
        stb
        imgui_glfw_vulkan)
target_compile_options(vulkan_renderer PRIVATE -Wall -Wextra -Werror)