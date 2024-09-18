#include <chrono>
#include "utils.h"
#include <windows.h>
#include <cstdlib>

void delay(int milliseconds) {
    auto start = std::chrono::high_resolution_clock::now();
    auto end = start;
    auto delay = std::chrono::milliseconds(milliseconds);

    while ((end - start) < delay) {
        end = std::chrono::high_resolution_clock::now();
    }
}

void updateCursorPos() { 
    GetCursorPos(&CursorPos);
    x = CursorPos.x;
    y = CursorPos.y;
}

void leftClick() {
    SendMessageW(GetForegroundWindow(), WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x, y));
    SendMessageW(GetForegroundWindow(), WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(x, y));
}

void rightClick() {
    SendMessageW(GetForegroundWindow(), WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(x, y));
    SendMessageW(GetForegroundWindow(), WM_RBUTTONUP, MK_RBUTTON, MAKELPARAM(x, y));
}

void toggleModes(Mode* mode) {
    switch (*mode) {
        case AUTOCLICK:
            *mode = MULTIPLIE;
            break;
        case MULTIPLIE:
            *mode = AUTOCLICK;
            break;
    }
}

ImVec4 convertRGB(int r, int g, int b) {
    return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
}

int range(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}