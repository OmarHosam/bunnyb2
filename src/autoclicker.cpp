#include "pch.h"
#include "utils/utils.h"
#include "clicker/autoclick.h"
#include "clicker/multiplie.h"
#include <imgui.h>

// TODO: Get imgui working properly
// so far it doesn't even get imported lol.

void handleModes() {
    // Left.
    switch (Settings::Modes::mode_left) {
    case AUTOCLICK:
        autoclick_left();
        break;
    case MULTIPLIE:
        multiplie_left();
        break;
    }

    // Right.
    switch (Settings::Modes::mode_right) {
    case AUTOCLICK:
        autoclick_right();
        break;
    case MULTIPLIE:
        multiplie_right();
        break;
    }
    
}

int main() {
    while (isRunning) {
        if (GetAsyncKeyState(Settings::exitKey)) {
            isRunning = false;
            break;
        }

        handleModes();
    }

    return 0;
}