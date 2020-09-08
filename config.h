#include "vswm.h"

#define DEBUG 1

#define BORDER_WIDTH 2
#define BORDER_ACTIVE_COLOR 0x000040
#define BORDER_INACTIVE_COLOR 0x101040

#define TITLEBAR_HEIGHT 18
#define TITLEBAR_ACTIVE_COLOR 0x000080
#define TITLEBAR_INACTIVE_COLOR 0x404040
#define TITLEBAR_DECORATION "/usr/include/X11/bitmaps/wide_weave"

#define TEXT_FONT "fixed"
#define TEXT_ACTIVE_COLOR 0xFFFFFF
#define TEXT_INACTIVE_COLOR 0x606060

#define MOVE_DELTA 20

#define SHADOW_X 3
#define SHADOW_Y 3
#define SHADOW_COLOR 0x202020

#define STATUS_TEXT_COLOR 0xFFFFFF

int MOVE_KEY = super;

combo keys[] = {
    { super, "c",   close,         0     },
    { super, "m",   maximize,      0     },
    { super, "Tab", switch_window, 0     },
    { super, "h",   move,          LEFT  },
    { super, "j",   move,          DOWN  },
    { super, "k",   move,          UP    },
    { super, "l",   move,          RIGHT },
    { super, "q",   logout,        0     },
};
