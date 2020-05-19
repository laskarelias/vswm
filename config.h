#include "vswm.h"

#define BORDER_COLOR 0x008080
#define BORDER_WIDTH 5

#define ctrl (1<<2)
#define alt (1<<3)
#define super (1<<6)
#define shift (1<<0)

combo keys[] = {
    { alt, "c", close    },
    { alt, "m", maximize },
};
