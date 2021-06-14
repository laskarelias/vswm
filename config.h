#define MOVE_KEY super

#define TEXT_FONT "-*-terminus-bold-r-normal-*-14-*-*-*-*-*-*-*"
#define TEXT_ACTIVE 0x000000

#define BORDER_WIDTH 2
#define BORDER_ACTIVE 0x008080
#define BORDER_INACTIVE 0x808080

#define INNER_BORDER_WIDTH 1
#define INNER_BORDER_ACTIVE 0x000000
#define INNER_BORDER_INACTIVE 0x808080

#define TITLEBAR_HEIGHT 21
#define TITLEBAR_ACTIVE 0x008080
#define TITLEBAR_INACTIVE 0x808080
#define TITLEBAR_BUTTONS 1
#define TITLEBAR_NAME 1

#define BUTTON_COLOR 0xc0c0c0
#define BUTTON_WIDTH 14
#define BUTTON_HEIGHT 14
#define BUTTON_SPACING 4

const key keys[] = {
    { super, "c",   close,  0},
    { super, "z",   info,   0},
};

const title_button title_buttons[] {
    { "V", close    },
    { "S", maximize },
    { "W", minimize },
    { "M", nothing }
};
