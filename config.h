#include "vswm.h"

#define ACTIVE_COLOR 0x800080
#define INACTIVE_COLOR 0x808080
#define BORDER_WIDTH 2

#define MOVE_DELTA 20

const char* TERM[] = {"xterm", 0};
const char* MENU[] = {"dmenu_run", 0};

combo keys[] = {
    { super, "x",   close_win,     {.i = 0     }},
    { super, "c",   center,        {.i = 0     }},
    { super, "m",   maximize,      {.i = 0     }},
    { super, "Tab", switch_window, {.i = 0     }},
    { super, "h",   move,          {.i = LEFT  }},
    { super, "j",   move,          {.i = DOWN  }},
    { super, "k",   move,          {.i = UP    }},
    { super, "l",   move,          {.i = RIGHT }},
    { super, "q",   logout,        {.i = 0     }},
    { super, "Return",   run,           {.c = TERM  }},
    { super, "a",   run,           {.c = MENU  }},

};
