cmake_minimum_required(VERSION 3.17)
project(particle_sys_particle_debug_shader)

set(SHADER_NAME particle)
set(SHADER_SOURCE_FOLDER
        ${CMAKE_CURRENT_SOURCE_DIR})
set(SHADER_RUNTIME_FOLDER
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/shaders/${SHADER_NAME})

#Filelist
set(SHADERS
        ${SHADER_SOURCE_FOLDER}/particle.frag
        ${SHADER_SOURCE_FOLDER}/particleMoveForward.comp
        ${SHADER_SOURCE_FOLDER}/particle.vert)
set(COMPILED_SHADERS
        ${SHADER_RUNTIME_FOLDER}/particle.frag.spv
        ${SHADER_RUNTIME_FOLDER}/particleMoveForward.comp.spv
        ${SHADER_RUNTIME_FOLDER}/particle.vert.spv)

file(MAKE_DIRECTORY ${SHADER_RUNTIME_FOLDER})
foreach (SHADER COMPILED_SHADER IN ZIP_LISTS SHADERS COMPILED_SHADERS)
    add_custom_command(
            OUTPUT ${COMPILED_SHADER}
            DEPENDS ${SHADER}
            COMMAND
            "${GLSLC_PROGRAM}"
            -o ${COMPILED_SHADER} -mfmt=bin -O
            --target-env=vulkan1.2
            ${SHADER}
            -Werror
            COMMENT "Building: ${SHADER}"
            VERBATIM
    )
endforeach ()

add_custom_target(particle_debug_shader DEPENDS ${COMPILED_SHADERS})
