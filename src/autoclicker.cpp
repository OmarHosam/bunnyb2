#include "pch.h"
#include "utils/utils.h"
#include "clicker/autoclick.h"
#include "clicker/multiplie.h"
#include "gui/gui.h"

// TODO: Get imgui working properly
// so far it doesn't even get imported lol.

void handleModes() {
    while (gui::isRunning) {
        if (GetAsyncKeyState(Settings::toggleKey)) {
            Settings::toggled = !Settings::toggled;
            delay(300);
        }
        
        if (!Settings::toggled) continue;

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
}

int main() {
    // create gui
    gui::CreateHWindow("BunnyB2");
    gui::CreateDevice();
    gui::CreateImGui();

    std::thread logic(handleModes);

    while (gui::isRunning) {
        gui::BeginRender();
        gui::Render();
        gui::EndRender();

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    Settings::toggled = false;

    // destroy gui
    gui::DestroyImGui();
    gui::DestroyDevice();
    gui::DestroyHWindow();

    return 0;
}