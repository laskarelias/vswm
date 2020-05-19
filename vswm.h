typedef struct combos {
    int modifiers;
    char key[8];
    void (* function)(Display* dpy, XEvent ev);
} combo;

void lll(char msg[]);
void key_init();
void key_handler(Display* dpy, XEvent ev);
void maximize(Display* dpy, XEvent ev);
void close(Display* dpy, XEvent ev);
void move_l(Display* dpy, XEvent ev);
void move_r(Display* dpy, XEvent ev);
void move_u(Display* dpy, XEvent ev);
void move_d(Display* dpy, XEvent ev);
void logout(Display* dpy, XEvent ev);
