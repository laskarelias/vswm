/* VSWM - Very Small Window Manager
 * VSWM - Very Simple Window Manager
 *
 * Thanks - TinyWM
 * Thanks - SOWM
*/

#include <X11/Xlib.h>
#include <stdio.h>

#include "config.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define win_size(W, gx, gy, gw, gh) XGetGeometry(dpy, W, &(Window){0}, gx, gy, gw, gh, &(unsigned int){0}, &(unsigned int){0})

static unsigned int running = 1;
static Window active = 0;

void lll(char msg[]){
    FILE * fp;
    fp = fopen("log.txt", "a");
    fprintf(fp, "%s \n", msg);
    fclose(fp);
}

void key_handler(Display* dpy, XEvent ev) {
    for (int i = 0; i < sizeof(keys) / sizeof(* keys); i++) {
        if ( (keys[i].modifiers == ev.xkey.state) && (XKeysymToKeycode(dpy, XStringToKeysym(keys[i].key)) == ev.xkey.keycode) ) {
            keys[i].function(dpy, ev);
        }
    }
}

void key_init(Display* dpy) {
    for (int i = 0; i < sizeof(keys) / sizeof(* keys); i++) {
        XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym(keys[i].key)), keys[i].modifiers, DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
    } 
}

/* Functions */
void close(Display* dpy, XEvent ev) {
    if (active) {
        XSelectInput(dpy, active, NoEventMask);
        XKillClient(dpy, active); 
        active = 0;
    }
}

void maximize(Display* dpy, XEvent ev) {
    if (active) {
        XMoveResizeWindow(dpy, active, 0, 0, XDisplayWidth(dpy, DefaultScreen(dpy)) - 2 * BORDER_WIDTH, XDisplayHeight(dpy, DefaultScreen(dpy)) - 2 * BORDER_WIDTH);
    }
}

void move_l(Display* dpy, XEvent ev) {
    if (active) {
        XWindowAttributes attr;
        XGetWindowAttributes(dpy, active, &attr);
        XMoveResizeWindow(dpy, active, attr.x - MOVE_DELTA, attr.y, attr.width, attr.height);
        //XSetInputFocus(dpy, active, RevertToParent, CurrentTime);
    }
}

void move_r(Display* dpy, XEvent ev) {
    if (active) {
        XWindowAttributes attr;
        XGetWindowAttributes(dpy, active, &attr);
        XMoveResizeWindow(dpy, active, attr.x + MOVE_DELTA, attr.y, attr.width, attr.height);
        //XSetInputFocus(dpy, ev.xkey.subwindow, RevertToParent, CurrentTime);
    }
}

void move_u(Display* dpy, XEvent ev) {
    if (active) {
        XWindowAttributes attr;
        XGetWindowAttributes(dpy, active, &attr);
        XMoveResizeWindow(dpy, active, attr.x, attr.y - MOVE_DELTA, attr.width, attr.height);
    }
}

void move_d(Display* dpy, XEvent ev) {
    if (active) {
        XWindowAttributes attr;
        XGetWindowAttributes(dpy, active, &attr);
        XMoveResizeWindow(dpy, active, attr.x, attr.y + MOVE_DELTA, attr.width, attr.height);
    }
}

void logout(Display* dpy, XEvent ev) {
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

    XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureRedirectMask);
    XGrabButton(dpy, 1, Mod1Mask, DefaultRootWindow(dpy), True, ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
    XGrabButton(dpy, 3, Mod1Mask, DefaultRootWindow(dpy), True, ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
    key_init(dpy);

    start.subwindow = None;
    while(running) {
        XNextEvent(dpy, &ev);
        if(ev.type == ConfigureRequest) {
            XConfigureWindow(dpy, ev.xconfigurerequest.window, ev.xconfigurerequest.value_mask, &(XWindowChanges) {
                                .x = ev.xconfigurerequest.x,
                                .y = ev.xconfigurerequest.y,
                                .width = ev.xconfigurerequest.width,
                                .height = ev.xconfigurerequest.height,
                                .border_width = BORDER_WIDTH
                             });
        }
        if(ev.type == MapRequest) {
            wx = wy = 0;
            ww = wh = 0;
            win_size(ev.xmaprequest.window, &wx, &wy, &ww, &wh);
            XSelectInput(dpy, ev.xmaprequest.window, EnterWindowMask | FocusChangeMask);
            XMoveResizeWindow(dpy, ev.xmaprequest.window, wx, wy, ww, wh);
            XSetWindowBorderWidth(dpy, ev.xmaprequest.window, BORDER_WIDTH);
            XMapWindow(dpy, ev.xmaprequest.window);
            if (!active) { XSetInputFocus(dpy, ev.xmaprequest.window, RevertToParent, CurrentTime); }
        }

        if (ev.type == KeyPress) {
            key_handler(dpy, ev);
        }

        if (ev.type == EnterNotify) {
            XSetInputFocus(dpy, ev.xcrossing.window, RevertToParent, CurrentTime);
        }
        if (ev.type == DestroyNotify) {
            lll("destroynotify");
            XSetInputFocus(dpy, ev.xdestroywindow.window, RevertToParent, CurrentTime);
            if (active == ev.xdestroywindow.window) { active = 0; }
            XKillClient(dpy, ev.xdestroywindow.window);
        }

        if (ev.type == UnmapNotify) {
            XSelectInput(dpy, ev.xunmap.window, NoEventMask);
            XUnmapWindow(dpy, ev.xunmap.window);
        }
        
        if (ev.type == FocusIn) {
            active = ev.xfocus.window;
            XSetWindowBorder(dpy, ev.xfocus.window, ACTIVE_COLOR);
        }
        if (ev.type == FocusOut) {
            XSetWindowBorder(dpy, ev.xfocus.window, INACTIVE_COLOR);
        }


        if (ev.type == ButtonPress && ev.xbutton.subwindow != None) {
            XRaiseWindow(dpy, ev.xbutton.subwindow);
            XGetWindowAttributes(dpy, ev.xbutton.subwindow, &attr);
            start = ev.xbutton;
        }
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

