#define ctrl ControlMask
#define alt Mod1Mask
#define super Mod4Mask
#define shift ShiftMask

#define MOVE_KEY super

#define BORDER_WIDTH 4
#define BORDER_ACTIVE 0x008080
#define BORDER_INACTIVE 0x004040

#define TITLEBAR_HEIGHT 18
#define TITLEBAR_ACTIVE 0x007070
#define TITLEBAR_INACTIVE 0x002020

#define BUTTON_COLOR 0x00A0A0

const key keys[] = {
    { super, "c",   close,  0},
    { super, "z",   info,   0},
};
