#include "multiplie.h"

bool canLeftClick = false;
bool canRightClick = false;

void handleMultiplie(int button, bool& canClick, bool clickEnabled, void (*clickFunc)()) {
    if (clickEnabled) {
        // Check if button is pressed and can click
        if (GetAsyncKeyState(button) < 0 && !canClick) {
            canClick = true;
        }

        // Check if button is released and canClick is true
        if (GetAsyncKeyState(button) >= 0 && canClick) {
            updateCursorPos();

            for (int i = 1; i <= Settings::Multiplie::multiplier; i++) {
                delay(Settings::Multiplie::delay);
                clickFunc();
            }

            canClick = false;
        }
    }
}

void multiplie_left() {
    handleMultiplie(VK_LBUTTON, canLeftClick, Settings::canLeftClick, leftClick);
}

void multiplie_right() {
    handleMultiplie(VK_RBUTTON, canRightClick, Settings::canRightClick, rightClick);
}