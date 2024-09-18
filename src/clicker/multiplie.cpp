// I guess there's no better way for refactoring this code other than this.
#include "multiplie.h"
#include <windows.h>
#include "../utils/utils.h"
#include "../globals.h"

bool canLeftClick = false;
bool canRightClick = false;

struct ClickSettings {
    bool& internalCanClick;
    bool clickEnabled;
    void (*clickFunc)();
    int multiplier;
    int delay;
};

void handleMultiplie(int button, ClickSettings settings) {
    if (settings.clickEnabled) {
        // Check if the button is pressed and canClick is false (start clicking)
        if (GetAsyncKeyState(button) < 0 && !settings.internalCanClick) {
            settings.internalCanClick = true;
        }

        // Check if the button is released and internalCanClick is true (perform the clicks)
        if (GetAsyncKeyState(button) >= 0 && settings.internalCanClick) {
            updateCursorPos();

            for (int i = 1; i <= settings.multiplier; i++) {
                delay(settings.delay);  // Delay between each click
                settings.clickFunc();   // Perform the click
            }

            settings.internalCanClick = false;  // Reset internal click state
        }
    }
}

void multiplie_left() {
    ClickSettings leftSettings = {
        canLeftClick,
        Settings::canLeftClick, 
        leftClick,
        Settings::leftClickSettings.multiplie.multiplier, 
        Settings::leftClickSettings.multiplie.delay
    };
    handleMultiplie(VK_LBUTTON, leftSettings);
}

void multiplie_right() {
    ClickSettings rightSettings = {
        canRightClick,
        Settings::canRightClick, 
        rightClick,
        Settings::rightClickSettings.multiplie.multiplier, 
        Settings::rightClickSettings.multiplie.delay
    };
    handleMultiplie(VK_RBUTTON, rightSettings);
}