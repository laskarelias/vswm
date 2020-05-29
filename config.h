#include "vswm.h"

#define ACTIVE_COLOR 0xff0000
#define INACTIVE_COLOR 0x000080
#define BORDER_WIDTH 5
#define MOVE_DELTA 20

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
