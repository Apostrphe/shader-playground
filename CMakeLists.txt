cmake_minimum_required(VERSION 3.16)
project(shader_playground)

set(CMAKE_CXX_STANDARD 17)

# Find packages
find_package(OpenGL REQUIRED)

# Add executable with all ImGui sources
add_executable(shader_playground
        main.cpp
        libs/glad/src/glad.c
        libs/imgui/imgui.cpp
        libs/imgui/imgui_demo.cpp
        libs/imgui/imgui_draw.cpp
        libs/imgui/imgui_tables.cpp
        libs/imgui/imgui_widgets.cpp
        libs/imgui/backends/imgui_impl_glfw.cpp
        libs/imgui/backends/imgui_impl_opengl3.cpp
)

# Include directories (more explicit paths)
target_include_directories(shader_playground PRIVATE
        ${CMAKE_SOURCE_DIR}/include/
        ${CMAKE_SOURCE_DIR}/libs/glad/include/
        ${CMAKE_SOURCE_DIR}/libs/glfw/include/
        ${CMAKE_SOURCE_DIR}/libs/imgui/
        ${CMAKE_SOURCE_DIR}/libs/imgui/backends/
)

# Link libraries
target_link_libraries(shader_playground
        OpenGL::GL
        ${CMAKE_SOURCE_DIR}/libs/glfw/lib/libglfw3.a
        -static-libgcc -static-libstdc++
)

# Compiler flags
target_compile_options(shader_playground PRIVATE
        -Wall -Wextra
)