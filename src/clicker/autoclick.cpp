#include "autoclick.h"

void handleAutoclick(int button, bool canClick, void (*clickFunc)(), int cps) {
    if (GetAsyncKeyState(button) < 0 && canClick) {
        updateCursorPos();
        clickFunc();
        delay(1000 / cps);
    }
}

void autoclick_left() {
    handleAutoclick(VK_LBUTTON, Settings::canLeftClick, leftClick, Settings::Left::Autoclick::cps);
}

void autoclick_right() {
    handleAutoclick(VK_RBUTTON, Settings::canRightClick, rightClick, Settings::Right::Autoclick::cps);
}