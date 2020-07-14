#define ctrl (1<<2)
#define alt (1<<3)
#define super (1<<6)
#define shift (1<<0)

enum {LEFT, DOWN, UP, RIGHT};
enum {tall, wide};

typedef struct Args {
    int i;
    const char** c;
} arg;

typedef struct combos {
    int modifiers;
    char key[8];
    void (* function)(Display* dpy, XEvent ev, arg a);
    arg a; 
} combo;

typedef struct wins win;
typedef struct wins {
    Window window;
    int x, y;
    unsigned int w, h;
    int size;
    win* prev;
    win* next;
} win;

void lll(char msg[]);
void key_init();
void key_handler(Display* dpy, XEvent ev);
void event_handler(Display* dpy, XEvent ev);

void run(Display* dpy, XEvent ev, arg a);
void maximize(Display* dpy, XEvent ev, arg a);
void close_win(Display* dpy, XEvent ev, arg a);
void center(Display* dpy, XEvent ev, arg a);
void switch_window(Display* dpy, XEvent ev, arg a);
void move(Display* dpy, XEvent ev, arg a);
void logout(Display* dpy, XEvent ev, arg a);
