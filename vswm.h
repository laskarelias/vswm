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
