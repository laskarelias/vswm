#define ctrl (1<<2)
#define alt (1<<3)
#define super (1<<6)
#define shift (1<<0)

enum direction {LEFT, DOWN, UP, RIGHT};

typedef struct combos {
    int modifiers;
    char key[8];
    void (* function)(Display* dpy, XEvent ev, int arg);
    int arg; 
} combo;

void lll(char msg[]);
void key_init();
void key_handler(Display* dpy, XEvent ev);
void event_handler(Display* dpy, XEvent ev);

void maximize(Display* dpy, XEvent ev, int arg);
void close(Display* dpy, XEvent ev, int arg);
void switch_window(Display* dpy, XEvent ev, int arg);
void move(Display* dpy, XEvent ev, int arg);
void logout(Display* dpy, XEvent ev, int arg);
