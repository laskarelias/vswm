#define MOVE_KEY super

#define TEXT_FONT "-*-terminus-bold-r-normal-*-14-*-*-*-*-*-*-*"
#define TEXT_ACTIVE 0x000000
#define TEXT_INACTIVE 0x606060

#define OUTER_BORDER_WIDTH 1
#define FRAME_WIDTH 2
#define BORDER_ACTIVE 0x000000
#define BORDER_INACTIVE 0x606060
#define INNER_BORDER_WIDTH 1
#define INNER_BORDER_ACTIVE 0x000000
#define INNER_BORDER_INACTIVE 0x606060

#define TITLEBAR_HEIGHT 18
#define TITLEBAR_POSITION top
#define TITLEBAR_ACTIVE 0x008080
#define TITLEBAR_INACTIVE 0x808080
#define TITLEBAR_BUTTONS left
#define TITLEBAR_NAME right

#define BUTTON_ACTIVE 0xc0c0c0
#define BUTTON_INACTIVE 0x808080
#define BUTTON_BORDER_ACTIVE 0x000000
#define BUTTON_BORDER_INACTIVE 0x808080
#define BUTTON_BORDER_WIDTH 1
#define BUTTON_WIDTH 14
#define BUTTON_HEIGHT 14
#define BUTTON_SPACING 4

const key keys[] = {
    { super, "c",   close,  0},
    { super, "z",   info,   0}
};

const title_button title_buttons[] {
    { "V", close    },
    { "S", close    },
    { "W", close    },
    { "M", close    },
};
