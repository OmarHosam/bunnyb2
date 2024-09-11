#include "pch.h"
#include "globals.h"
#include "utils/utils.h"
#include "clicker/autoclick.h"
#include "clicker/multiplie.h"

int main() {
    enum Mode mode = AUTOCLICK;

    while (!GetAsyncKeyState(Settings::exitKey)) {
        switch (mode) {
        case AUTOCLICK:
            autoclick();
            break;
        case MULTIPLIE:
            multiplie();
            break;
        }
    }

    return 0;
}