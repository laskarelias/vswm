#include "vswm.h"

#define ACTIVE_COLOR 0x008080
#define INACTIVE_COLOR 0x808080
#define BORDER_WIDTH 5
#define TITLEBAR_HEIGHT 20
#define TITLEBAR_COLOR 0x008080

#define MOVE_DELTA 20

#define SHADOW_X 30
#define SHADOW_Y 30
#define SHADOW_COLOR 0x202020

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
