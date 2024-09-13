enum Mode
{
    AUTOCLICK,
    MULTIPLIE,
};

namespace Settings {
    namespace Autoclick {
        int cps = 15;
    };
    namespace Multiplie {
        int multiplier = 1;
        int delay = 30;
    };
    int exitKey = 0x50;
    bool canRightClick = true;
    bool canLeftClick = true;
    namespace Modes {
        enum Mode mode_left = MULTIPLIE;
        enum Mode mode_right = AUTOCLICK;
    };
};

bool isRunning = true;


int x;
int y;
POINT CursorPos;