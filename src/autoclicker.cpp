#include "pch.h"

#include "utils/utils.h"
#include "clicker/autoclick.h"
#include "clicker/multiplie.h"
#include "gui/gui.h"

// Function to toggle settings based on hotkey press
void handleToggle() {
    if (GetAsyncKeyState(Settings::toggleKey)) {
        Settings::toggled = !Settings::toggled;
        delay(300);  // Small delay to prevent rapid toggling
    }
}

// Main logic thread to manage modes and toggle
void handleModes() {
    while (gui::isRunning) {
        handleToggle();

        if (!Settings::toggled) continue;  // Skip processing if not toggled

        // Left
        switch (Settings::leftClickSettings.mode) {
            case AUTOCLICK:
                autoclick_left();
                break;
            case MULTIPLIE:
                multiplie_left();
                break;
        }

        // Right
        switch (Settings::rightClickSettings.mode) {
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
    // Initialize GUI
    gui::CreateHWindow("BunnyB2");
    gui::CreateDevice();
    gui::CreateImGui();

    // Start the logic handling thread
    std::thread logic(handleModes);

    // Main render loop
    while (gui::isRunning) {
        gui::BeginRender();  // Start GUI rendering
        gui::Render();       // Render GUI elements
        gui::EndRender();    // End GUI rendering

        std::this_thread::sleep_for(std::chrono::milliseconds(5));  // Limit CPU usage
    }

    Settings::toggled = false;  // Ensure settings are disabled on exit

    // Cleanup GUI
    gui::DestroyImGui();
    gui::DestroyDevice();
    gui::DestroyHWindow();

    // Join logic thread before exit
    if (logic.joinable()) {
        logic.join();
    }

    return 0;
}