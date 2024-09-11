#include <chrono>
#include "utils.h"

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