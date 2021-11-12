cmake_minimum_required(VERSION 3.17)
project(lib_app_info)

execute_process(
        COMMAND git log -1 --format=%h
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE)

add_library(app_info INTERFACE)
target_include_directories(app_info
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/public)
target_compile_definitions(app_info INTERFACE
        VERSION_MAJOR=${APP_VERSION_MAJOR}
        VERSION_MINOR=${APP_VERSION_MINOR}
        VERSION_PATCH=${APP_VERSION_PATCH}
        BINARY_NAME="${APP_BINARY_NAME}"
        COMMIT_HASH="${GIT_COMMIT_HASH}")