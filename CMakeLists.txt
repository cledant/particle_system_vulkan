cmake_minimum_required(VERSION 3.17)
project(particle_system_vulkan)

#App info
set(APP_BINARY_NAME particle_system)
set(APP_VERSION_MAJOR 1)
set(APP_VERSION_MINOR 0)
set(APP_VERSION_PATCH 0)

#Detecting + setting global LTO
include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported OUTPUT lto_output)

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND lto_supported)
    message("Compiling with LTO enabled")
    cmake_policy(SET CMP0069 NEW)
    set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else ()
    message("LTO not supported")
endif ()

#Runtime resources
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#Checking Vulkan
find_package(Vulkan REQUIRED FATAL_ERROR)
find_program(GLSLC_PROGRAM glslc REQUIRED FATAL_ERROR)

#Setting global C/CXX compile options
set(OPTIMIZATION_TYPE NATIVE CACHE STRING "")

if (${OPTIMIZATION_TYPE} STREQUAL NATIVE)
    message("Build binary for current computer arch")
    set(CMAKE_CXX_FLAGS "-march=native -pipe")
    set(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS})
elseif (${OPTIMIZATION_TYPE} STREQUAL AVX2)
    message("Build binary with AVX2 profile")
    set(CMAKE_CXX_FLAGS "-march=core2 -mtune=haswell \
    -mmmx -msse -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2 \
    -mavx -mavx2 -mbmi -mbmi2 -mf16c -mfma -mfsgsbase \
    -mlzcnt -mmovbe -mpclmul -mpopcnt -maes \
    -mrdrnd  -pipe")
    set(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS})
elseif (${OPTIMIZATION_TYPE} STREQUAL SSE4)
    message("Build binary with SSE4 profile")
    set(CMAKE_CXX_FLAGS "-march=core2 -mtune=nehalem \
    -mmmx -msse -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2 \
    -mpopcnt -pipe")
    set(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS})
else ()
    message("No arch optimization selected")
    set(CMAKE_CXX_FLAGS "-pipe")
    set(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS})
endif ()

#Setup glm
add_library(glm INTERFACE IMPORTED)
set_target_properties(glm PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/external/glm)

#Setup GLFW
option(GLFW_BUILD_EXAMPLES "" OFF)
option(GLFW_BUILD_TESTS "" OFF)
option(GLFW_INSTALL "" OFF)
option(BUILD_SHARED_LIBS "" OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/glfw ${CMAKE_BINARY_DIR}/external/glfw/build)

#Setup stb
add_library(stb INTERFACE IMPORTED)
set_target_properties(stb PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/external/stb)

#Setup fmt
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/fmt ${CMAKE_BINARY_DIR}/external/fmt/build)

#Setup imgui
set(IMGUI_FOLDER ${CMAKE_SOURCE_DIR}/external/imgui)
add_library(imgui_glfw_vulkan STATIC
        ${IMGUI_FOLDER}/backends/imgui_impl_glfw.cpp
        ${IMGUI_FOLDER}/backends/imgui_impl_vulkan.cpp
        ${IMGUI_FOLDER}/imgui.cpp
        ${IMGUI_FOLDER}/imgui_draw.cpp
        ${IMGUI_FOLDER}/imgui_demo.cpp
        ${IMGUI_FOLDER}/imgui_tables.cpp
        ${IMGUI_FOLDER}/imgui_widgets.cpp)
target_include_directories(imgui_glfw_vulkan
        PUBLIC
        ${IMGUI_FOLDER}
        ${IMGUI_FOLDER}/backend)
add_dependencies(imgui_glfw_vulkan
        glfw)
target_link_libraries(imgui_glfw_vulkan
        glfw)

#Project libs
add_subdirectory(libraries/io_manager)
add_subdirectory(libraries/engine)
add_subdirectory(libraries/vulkan_renderer)
add_subdirectory(libraries/ui)
add_subdirectory(libraries/app_info)
add_subdirectory(libraries/model_instance)

#Shaders
add_subdirectory(shaders/skybox)
add_subdirectory(shaders/particle)

#Skybox textures
set(SKYBOX_TEXTURE_SOURCE_FOLDER
        ${CMAKE_SOURCE_DIR}/textures/skybox)
set(SKYBOX_TEXTURE_RUNTIME_FOLDER
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/textures)
file(COPY ${SKYBOX_TEXTURE_SOURCE_FOLDER}
        DESTINATION
        ${SKYBOX_TEXTURE_RUNTIME_FOLDER})

#Main binary
add_executable(${APP_BINARY_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/binary/private/particle_system.cpp)
target_include_directories(${APP_BINARY_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/binary/private)
target_link_libraries(${APP_BINARY_NAME} iomanager engine vulkan)
add_dependencies(${APP_BINARY_NAME} iomanager engine skybox_shader particle_shader ui)
set_target_properties(${APP_BINARY_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)
target_compile_options(${APP_BINARY_NAME} PRIVATE -Wall -Wextra -Werror)
