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
}

int x;
int y;
POINT CursorPos;