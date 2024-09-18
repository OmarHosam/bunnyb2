#pragma once

#include <d3d9.h>
#include <imgui.h>
#include "../utils/utils.h"
#include "../globals.h"

namespace gui
{
    // constant window size
    constexpr int WIDTH = 500;
    constexpr int HEIGHT = 300;

    // For checking if window is not focused right now.
    inline bool isWindowNotFocused = false;

    // Storing our HWND because ImGui::IsWindowFocused is so damn buggy.
    static HWND hwnd;

    // when this changes, exit threads
    // and close menu :)
    inline bool isRunning = true;

    // winapi window vars
    inline HWND window = nullptr;
    inline WNDCLASSEX windowClass = { };

    // points for window movement
    inline POINTS position = { };

    // direct x state vars
    inline PDIRECT3D9 d3d = nullptr;
    inline LPDIRECT3DDEVICE9 device = nullptr;
    inline D3DPRESENT_PARAMETERS presentParameters = { };

    // handle window creation & destruction
    void CreateHWindow(const char* windowName) noexcept;
    void DestroyHWindow() noexcept;

    // handle device creation & destruction
    bool CreateDevice() noexcept;
    void ResetDevice() noexcept;
    void DestroyDevice() noexcept;

    // handle ImGui creation & destruction
    void CreateImGui() noexcept;
    void DestroyImGui() noexcept;

    void BeginRender() noexcept;
    void EndRender() noexcept;
    void Render() noexcept;
    void Style() noexcept;
}

namespace Color {
    // Gruvbox color palette
    static ImVec4 bg0 = convertRGB(29, 32, 33);      // background
    static ImVec4 bg1 = convertRGB(40, 40, 40);      // light background
    static ImVec4 bg2 = convertRGB(50, 48, 47);      // lighter background
    static ImVec4 fg = convertRGB(235, 219, 178);    // foreground
    static ImVec4 gray = convertRGB(146, 131, 116);  // gray
    static ImVec4 red = convertRGB(204, 36, 29);     // red
    static ImVec4 green = convertRGB(152, 151, 26);  // green
    static ImVec4 yellow = convertRGB(215, 153, 33); // yellow
    static ImVec4 blue = convertRGB(69, 133, 136);   // blue
    static ImVec4 purple = convertRGB(177, 98, 134); // purple
    static ImVec4 aqua = convertRGB(104, 157, 106);  // aqua
    static ImVec4 orange = convertRGB(214, 93, 14);  // orange
}

// #include "gui.cpp" // ~~I fucking hate Meson.~~ cmake on top!! (with the help of chatgpt)