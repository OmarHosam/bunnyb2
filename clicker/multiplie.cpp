#include "multiplie.h"

bool canClick = false;

void multiplie() {
    if (GetAsyncKeyState(VK_LBUTTON) < 0 && !canClick) {
        canClick = true;
    }

    if (GetAsyncKeyState(VK_LBUTTON) >= 0 && canClick) {
        updateCursorPos();

        for (int i = 1; i <= Settings::Multiplie::multiplier; i++) {
            delay(Settings::Multiplie::delay);
            leftClick();
        }

        canClick = false;
    }
}