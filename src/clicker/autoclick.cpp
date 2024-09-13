#include "autoclick.h"

void handleAutoclick(int button, bool canClick, void (*clickFunc)()) {
    if (GetAsyncKeyState(button) < 0 && canClick) {
        updateCursorPos();
        clickFunc();
        delay(1000 / Settings::Autoclick::cps);
    }
}

void autoclick_left() {
    handleAutoclick(VK_LBUTTON, Settings::canLeftClick, leftClick);
}

void autoclick_right() {
    handleAutoclick(VK_RBUTTON, Settings::canRightClick, rightClick);
}