cmake_minimum_required(VERSION 3.15)

# Project name and version
project(BunnyB2 VERSION 0.1)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define the source directories
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/ext/imgui)

# Manually list the ImGui sources
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_impl_dx10.cpp
    ${IMGUI_DIR}/imgui_impl_dx9.cpp
    ${IMGUI_DIR}/imgui_impl_win32.cpp
)

# Add the source files from your project
file(GLOB_RECURSE SRC_FILES
    "${SRC_DIR}/*.cpp"
    "${SRC_DIR}/*.h"
)

# Compile ImGui as a static library
add_library(ImGui STATIC ${IMGUI_SOURCES})

# Add include directories for ImGui
target_include_directories(ImGui PRIVATE ${IMGUI_DIR})

# Create the main executable
add_executable(BunnyB2 WIN32 ${SRC_FILES})

# Add include directories
target_include_directories(BunnyB2 PRIVATE
    ${SRC_DIR}
    ${IMGUI_DIR}
)

# Link the ImGui static library to the main executable
target_link_libraries(BunnyB2 PRIVATE ImGui)

# Link DirectX dependencies (DirectX 9, 10, and DWM)
if (WIN32)
    target_link_libraries(BunnyB2 PRIVATE d3d9 d3d10 dwmapi)
endif()
