#include "autoclick.h"
#include <windows.h>
#include "../utils/utils.h"
#include "../globals.h"
#include <iostream>

void handleAutoclick(int button, bool canClick, void (*clickFunc)(), Settings::ClickSettings settings) {
    if (GetAsyncKeyState(button) < 0 && canClick) {
        updateCursorPos();  // Update cursor position
        clickFunc();        // Execute the click function
        if (settings.autoclick.jitter) {
            int jitterIntensity = settings.autoclick.jitterIntensity;
            SetCursorPos(x + range((jitterIntensity * -1), jitterIntensity), y + range((jitterIntensity * -1), jitterIntensity));
        }
        delay(1000 / settings.autoclick.cps);  // Delay based on CPS
    }
}

void autoclick_left() {
    handleAutoclick(VK_LBUTTON,
        Settings::canLeftClick,
        leftClick,
        Settings::leftClickSettings
    );
}

void autoclick_right() {
    handleAutoclick(VK_RBUTTON,
        Settings::canRightClick,
        rightClick,
        Settings::rightClickSettings
    );
}
