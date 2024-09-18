#pragma once

#include <imgui.h>
#include <windows.h>
#include "../globals.h"

void delay(int milliseconds);
void updateCursorPos();
void leftClick();
void rightClick();
void toggleModes(Mode* mode);
ImVec4 convertRGB(int r, int g, int b);
int range(int min, int max);