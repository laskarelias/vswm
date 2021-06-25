#define MOVE_KEY super

#define TEXT_FONT "-*-terminus-bold-r-normal-*-14-*-*-*-*-*-*-*"
#define TEXT_ACTIVE 0x000000

#define OUTER_BORDER_WIDTH 1
#define OUTER_BORDER_ACTIVE 0x000000
#define OUTER_BORDER_INACTIVE 0x000000

#define BORDER_WIDTH 2
#define BORDER_ACTIVE 0x000000
#define BORDER_INACTIVE 0x000000

#define INNER_BORDER_WIDTH 1
#define INNER_BORDER_ACTIVE 0x000000
#define INNER_BORDER_INACTIVE 0x404040

#define TITLEBAR_HEIGHT 18
#define TITLEBAR_POSITION bottom
#define TITLEBAR_ACTIVE 0xa0FFFF
#define TITLEBAR_INACTIVE 0x808080
#define TITLEBAR_BUTTONS no
#define TITLEBAR_NAME center

#define BUTTON_COLOR 0xc0c0c0
#define BUTTON_WIDTH 20
#define BUTTON_HEIGHT 14
#define BUTTON_SPACING 5

const key keys[] = {
    { super, "c",   close,  0},
    { super, "z",   info,   0}
};

const title_button title_buttons[] {
    { " ", close    },
    { " ", close    },
    { " ", close    },
};
