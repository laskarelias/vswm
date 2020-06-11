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
    if (active) {
        XSelectInput(dpy, active, NoEventMask);
        XKillClient(dpy, active.win); 
        active = 0;
    }
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
            case DOWN:
                XMoveResizeWindow(dpy, active, attr.x, attr.y + MOVE_DELTA, attr.width, attr.height);
            case UP:
                XMoveResizeWindow(dpy, active, attr.x, attr.y - MOVE_DELTA, attr.width, attr.height);
            case RIGHT:
                XMoveResizeWindow(dpy, active, attr.x + MOVE_DELTA, attr.y, attr.width, attr.height);
        }
    }
}

void logout(Display* dpy, XEvent ev, int arg) {
    running = 0;
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

