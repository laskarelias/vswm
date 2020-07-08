/* VSWM - Very Small Window Manager
 * VSWM - Very Simple Window Manager
 *
 * Thanks - TinyWM
 * Thanks - SOWM
*/

#include <X11/Xlib.h>
#include <stdio.h>

#include "config.h"
#include <X11/bitmaps/boxes>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define win_size(W, gx, gy, gw, gh) XGetGeometry(dpy, W, &(Window){0}, gx, gy, gw, gh, &(unsigned int){0}, &(unsigned int){0})

static unsigned int running = 1;
static win *win_list = {0};
static win *active = {0};

void lll(char msg[]){
    FILE * fp;
    fp = fopen("log.txt", "a");
    fprintf(fp, "[%ld] - %s \n", CurrentTime, msg);
    fclose(fp);
}

int error_handler(Display* dpy, XErrorEvent* ev){
   lll("[ ERROR ]");
   char err[48];
   XGetErrorText(dpy, ev->error_code, err, 48);
//   FILE * fp = fopen("log.txt", "a");
//   fprintf(fp, "%s \n", ev->error_code);
//   fclose(fp);
   lll(err);
   return 0;
}

void key_handler(Display* dpy, XEvent ev) {
    for (int i = 0; i < sizeof(keys) / sizeof(* keys); i++) {
        if ( (keys[i].modifiers == ev.xkey.state) && (XKeysymToKeycode(dpy, XStringToKeysym(keys[i].key)) == ev.xkey.keycode) ) {
            keys[i].function(dpy, ev, keys[i].arg);
        }
    }
}

void key_init(Display* dpy) {
    for (int i = 0; i < sizeof(keys) / sizeof(* keys); i++) {
        XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym(keys[i].key)), keys[i].modifiers, DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
    } 
}

/* Functions */
void close(Display* dpy, XEvent ev, int arg) {
//    if (active) {
//        XSelectInput(dpy, active, NoEventMask);
//        XKillClient(dpy, active.win); 
//        active = 0;
//    }
}

void maximize(Display* dpy, XEvent ev, int arg) {
    if (active) {
        XWindowAttributes attr;
        XGetWindowAttributes(dpy, active, &attr);
        if (attr.width == XDisplayWidth(dpy, DefaultScreen(dpy)) - 2 * BORDER_WIDTH && attr.height == XDisplayHeight(dpy, DefaultScreen(dpy)) - 2 * BORDER_WIDTH) {
            XMoveResizeWindow(dpy, active, XDisplayWidth(dpy, DefaultScreen(dpy)) / 3, XDisplayHeight(dpy, DefaultScreen(dpy)) / 3, (XDisplayWidth(dpy, DefaultScreen(dpy)) - 2 * BORDER_WIDTH) / 3, (XDisplayHeight(dpy, DefaultScreen(dpy)) - 2 * BORDER_WIDTH) / 3);
        } else {
            XMoveResizeWindow(dpy, active, 0, 0, XDisplayWidth(dpy, DefaultScreen(dpy)) - 2 * BORDER_WIDTH, XDisplayHeight(dpy, DefaultScreen(dpy)) - 2 * BORDER_WIDTH);
        }
    }
}

void switch_window(Display* dpy, XEvent ev, int arg) {
    XLowerWindow(dpy, active);
}

void move(Display* dpy, XEvent ev, int arg) {
    if (active) {
        XWindowAttributes attr;
        XGetWindowAttributes(dpy, active, &attr);
        switch(arg) {
            case LEFT:
                XMoveResizeWindow(dpy, active, attr.x - MOVE_DELTA, attr.y, attr.width, attr.height);
                break;
            case DOWN:
                XMoveResizeWindow(dpy, active, attr.x, attr.y + MOVE_DELTA, attr.width, attr.height);
                break;
            case UP:
                XMoveResizeWindow(dpy, active, attr.x, attr.y - MOVE_DELTA, attr.width, attr.height);
                break;
            case RIGHT:
                XMoveResizeWindow(dpy, active, attr.x + MOVE_DELTA, attr.y, attr.width, attr.height);
                break;
        }
    }
}

void logout(Display* dpy, XEvent ev, int arg) {
    running = 0;
}

void event_handler(Display* dpy, XEvent ev) {
    switch (ev.type) {
        case ConfigureRequest:
            XConfigureWindow(dpy, ev.xconfigurerequest.window, ev.xconfigurerequest.value_mask, &(XWindowChanges) {
                .x = ev.xconfigurerequest.x,
                .y = ev.xconfigurerequest.y,
                .width = ev.xconfigurerequest.width,
                .height = ev.xconfigurerequest.height,
                .border_width = BORDER_WIDTH
            });
            break;
        case MapRequest:
            //win_size(ev.xmaprequest.window, &wx, &wy, &ww, &wh);
            XSelectInput(dpy, ev.xmaprequest.window, StructureNotifyMask | EnterWindowMask | FocusChangeMask);
            //XMoveResizeWindow(dpy, ev.xmaprequest.window, wx, wy, ww, wh);
            
            XSetWindowBorderWidth(dpy, ev.xmaprequest.window, BORDER_WIDTH);
            XSetWindowBorder(dpy, ev.xmaprequest.window, INACTIVE_COLOR);
            XMapWindow(dpy, ev.xmaprequest.window); 
            break;
        case KeyPress:
            key_handler(dpy, ev);
            break;
        case EnterNotify:
            XSetInputFocus(dpy, ev.xcrossing.window, RevertToParent, CurrentTime);
            active = ev.xcrossing.subwindow;
            break;
        case DestroyNotify:
            XSelectInput(dpy, ev.xdestroywindow.window, NoEventMask);
            break;
        case UnmapNotify:
            XSelectInput(dpy, ev.xunmap.window, NoEventMask);
            XUnmapWindow(dpy, ev.xunmap.window);
            break;
        case FocusIn:
            XSetWindowBorder(dpy, ev.xfocus.window, ACTIVE_COLOR);
            break;
        case FocusOut:
            XSetWindowBorder(dpy, ev.xfocus.window, INACTIVE_COLOR);
            break;
        default:
            break;
    }
}

int main(void)
{
    static Display* dpy;
    XWindowAttributes attr;
    XButtonEvent start;
    static XEvent ev;

    int wx, wy;
    unsigned int ww, wh;

    if(!(dpy = XOpenDisplay(0x0))) return 1;
    lll("session");
    XSetErrorHandler(error_handler);

    XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureRedirectMask);
    XGrabButton(dpy, 1, Mod1Mask, DefaultRootWindow(dpy), True, ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
    XGrabButton(dpy, 3, Mod1Mask, DefaultRootWindow(dpy), True, ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
    key_init(dpy);

    XDefineCursor(dpy, DefaultRootWindow(dpy), XCreateFontCursor(dpy, 68));
    start.subwindow = None;
    while(running) {
        XNextEvent(dpy, &ev);

        event_handler(dpy, ev);

        if(ev.type == MotionNotify && start.subwindow != None) {
            int xdiff = ev.xbutton.x_root - start.x_root;
            int ydiff = ev.xbutton.y_root - start.y_root;
            XMoveResizeWindow(dpy, start.subwindow,
                attr.x + (start.button==1 ? xdiff : 0),
                attr.y + (start.button==1 ? ydiff : 0),
                MAX(1, attr.width + (start.button==3 ? xdiff : 0)),
                MAX(1, attr.height + (start.button==3 ? ydiff : 0)));
        }
        else if(ev.type == ButtonRelease)
            start.subwindow = None;
    }
}

