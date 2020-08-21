#include "vswm.h"

#define BORDER_WIDTH 2
#define BORDER_ACTIVE_COLOR 0x008080
#define BORDER_INACTIVE_COLOR 0x404040

#define TITLEBAR_HEIGHT 10
#define TITLEBAR_ACTIVE_COLOR 0x008080
#define TITLEBAR_INACTIVE_COLOR 0x404040

#define MOVE_DELTA 20

#define SHADOW_X 3
#define SHADOW_Y 3
#define SHADOW_COLOR 0x000000

int MOVE_KEY = super;

combo keys[] = {
    { super, "c",   close,         0     },
    { super, "m",   maximize,      0     },
    { super, "Tab", switch_window, 0     },
    { super, "h",   move,          LEFT  },
    { super, "j",   move,          DOWN  },
    { super, "k",   move,          UP,   },
    { super, "l",   move,          RIGHT },
    { super, "q",   logout,        0     },
};
