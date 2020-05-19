#include "vswm.h"

#define BORDER_COLOR 0x808080
#define BORDER_WIDTH 2
#define MOVE_DELTA 20

#define ctrl (1<<2)
#define alt (1<<3)
#define super (1<<6)
#define shift (1<<0)

combo keys[] = {
    { alt | shift, "c", close    },
    { alt, "m", maximize },
    { super, "h", move_l	 },
    { super, "j", move_d	 },
    { super, "k", move_u	 },
    { super, "l", move_r	 },
};
