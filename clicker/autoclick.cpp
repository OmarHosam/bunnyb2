#include "autoclick.h"

void autoclick() {
    if (GetAsyncKeyState(VK_LBUTTON) < 0) {
        updateCursorPos();

        leftClick();

        delay((1000 / Settings::Autoclick::cps));
    }
}