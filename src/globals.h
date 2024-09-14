enum Mode
{
    AUTOCLICK,
    MULTIPLIE,
};

namespace Settings {
    namespace Left {
        namespace Autoclick {
            int cps = 15;
        };
        namespace Multiplie {
            int multiplier = 1;
            int delay = 30;
        };
    }

    namespace Right {
        namespace Autoclick {
            int cps = 15;
        };
        namespace Multiplie {
            int multiplier = 1;
            int delay = 30;
        };
    }
    
    int toggleKey = 0x50;
    bool canRightClick = true;
    bool canLeftClick = false;
    namespace Modes {
        enum Mode mode_left = MULTIPLIE;
        enum Mode mode_right = AUTOCLICK;
    };
    bool toggled = true;
};

int x;
int y;
POINT CursorPos;