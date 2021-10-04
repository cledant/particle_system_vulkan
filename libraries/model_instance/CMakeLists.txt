cmake_minimum_required(VERSION 3.17)
project(lib_model_instance)

add_library(model_instance STATIC
        private/ModelInstanceInfo.cpp)
target_include_directories(model_instance
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/public
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)
add_dependencies(model_instance glm)
target_link_libraries(model_instance PUBLIC glm)
set_target_properties(model_instance PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)
target_compile_options(model_instance PRIVATE -Wall -Wextra -Werror)