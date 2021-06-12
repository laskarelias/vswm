#define ctrl ControlMask
#define alt Mod1Mask
#define super Mod4Mask
#define shift ShiftMask

#define MOVE_KEY super

#define TEXT_FONT "-*-terminus-bold-r-normal-*-14-*-*-*-*-*-*-*"
#define TEXT_ACTIVE 0x000000

#define BORDER_WIDTH 1
#define BORDER_ACTIVE 0x000000
#define BORDER_INACTIVE 0x000000

#define INNER_BORDER_WIDTH 1
#define INNER_BORDER_ACTIVE 0x000000
#define INNER_BORDER_INACTIVE 0x000000

#define TITLEBAR_HEIGHT 20
#define TITLEBAR_ACTIVE 0x000080
#define TITLEBAR_INACTIVE 0x808080

#define BUTTON_COLOR 0xc0c0c0
#define BUTTON_WIDTH 14
#define BUTTON_HEIGHT 14
#define BUTTON_SPACING 4

const key keys[] = {
    { super, "c",   close,  0},
    { super, "z",   info,   0},
};

const title_button title_buttons[] {
    { "X", close    },
    { "O", maximize },
    { "-", minimize }
};
