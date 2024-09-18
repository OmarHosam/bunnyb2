#pragma once

#include <windows.h>

enum Mode {
    AUTOCLICK,
    MULTIPLIE,
};

namespace Settings {
    inline struct ClickSettings {
        struct AutoclickSettings {
            int cps = 15;
            bool jitter = false;
            int jitterIntensity = 3;
        } autoclick;

        struct MultiplieSettings {
            int multiplier = 1;
            int delay = 30;
        } multiplie;

        Mode mode = AUTOCLICK;
    } leftClickSettings, rightClickSettings;

    inline int toggleKey = 0x50;
    inline bool canRightClick = false;
    inline bool canLeftClick = false;

    inline bool disableOnFocus = false;

    inline bool toggled = false; // General toggle flag.
}

inline int x, y;
inline POINT CursorPos;