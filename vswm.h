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

typedef struct titlebar_buttons {
    char c[8];
    void (* function)(Display* dpy, XEvent ev, int arg);
    int arg; 
} titlebar_button;

typedef struct btns btn;
typedef struct btns {
    Window window;
    int x, y;
    unsigned int w, h;
    GC gc;
    btn* prev;
    btn* next;
} btn;

typedef struct wins win;
typedef struct wins {
    Window window;
    int x, y;
    unsigned int w, h;
    int size;
    win* prev;
    win* next;
    btn* btn_list;
    Window t;
    Window s;
    GC gc;
} win;

void _restack(Display* dpy, win* w);
void _focus(Display* dpy, win* w, int a);
void _destroy_decorations(Display* dpy, win* w);
void _move(Display* dpy, win* w, int btn, int dx, int dy);
void _text(Display* dpy, win* w);
void _status(Display* dpy);
void _refresh_bar(Display* dpy);
void _text2(Display* dpy, btn b, XTextProperty name, int x, int y, int pad, unsigned long fg, unsigned long bg);
void _get_name(Display* dpy, Window win, XTextProperty* name, int* d, int* asc, int* desc, XCharStruct* overall);
btn  _btn(Display* dpy, Window parent, int x, int y, unsigned int w, unsigned int h, unsigned long b, unsigned long fg, unsigned long bg);
void _refresh_btn(Display* dpy, win* b, char* text);



// void _button(Display* dpy, win* w, int x, int y, unsigned int width, unsigned int h, int i);

void lll(char msg[]);
int  error_handler(Display* dpy, XErrorEvent* ev);
void key_init();
void key_handler(Display* dpy, XEvent ev);
void event_handler(Display* dpy, XEvent ev);

void maximize(Display* dpy, XEvent ev, int arg);
void close(Display* dpy, XEvent ev, int arg);
void center(Display* dpy, XEvent ev, int arg);
void switch_window(Display* dpy, XEvent ev, int arg);
void move(Display* dpy, XEvent ev, int arg);
void logout(Display* dpy, XEvent ev, int arg);
