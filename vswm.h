#define ctrl ControlMask
#define alt Mod1Mask
#define super Mod4Mask
#define shift ShiftMask

enum {LEFT, DOWN, UP, RIGHT};
enum {tall, wide};
enum {INACTIVE, ACTIVE};

typedef struct combos {
    int modifiers;
    char key[8];
    void (* function)(Display* dpy, XEvent ev, int arg);
    int arg; 
} combo;

typedef struct wins win;

typedef struct wins {
    Window window;
    int x, y;
    unsigned int w, h;
    int size;
    win* prev;
    win* next;
    Window t;
    Window s;
    char* title;
    GC gc;
} win;

void _focus(Display* dpy, win* w, int a);
void _destroy_decorations(Display* dpy, win* w);
void _text(Display* dpy, win* w);

void lll(char msg[]);
void key_init();
void key_handler(Display* dpy, XEvent ev);
void event_handler(Display* dpy, XEvent ev);

void maximize(Display* dpy, XEvent ev, int arg);
void close(Display* dpy, XEvent ev, int arg);
void switch_window(Display* dpy, XEvent ev, int arg);
void move(Display* dpy, XEvent ev, int arg);
void logout(Display* dpy, XEvent ev, int arg);

