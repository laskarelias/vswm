/*                         
__   _______      ___ __ ___  
\ \ / / __\ \ /\ / / '_ ` _ \ 
 \ V /\__ \\ V  V /| | | | | |
  \_/ |___/ \_/\_/ |_| |_| |_|

*/

/* VSWM - Very Small Window Manager
 * VSWM - Very Simple Window Manager
 *
 * Thanks - TinyWM
 * Thanks - SOWM
*/

#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define win_size(W, gx, gy, gw, gh) XGetGeometry(dpy, W, &(Window){0}, gx, gy, gw, gh, &(unsigned int){0}, &(unsigned int){0})
#define ALL_WINDOWS win *t = 0, *w = win_list; w && t != win_list->prev; t = w, w = w->next
static unsigned int running = 1;

static win* win_list = {0};
static win* active = {0};
static Window w_arr[3];
static Display* dpy;
XButtonEvent start;
static XEvent ev;

char i2msg[12];

void lll(char msg[]){
    FILE * fp;
    fp = fopen("log.txt", "a");
    fprintf(fp, "[%ld] - %s \n", CurrentTime, msg);
    fclose(fp);
}

int error_handler(Display* dpy, XErrorEvent* ev){
   lll("[ ERROR ]");
   FILE * fp = fopen("log.txt", "a");
   fprintf(fp, "%d \n", ev->error_code);
   fclose(fp);
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

/* Helpers */ 
void _focus(Display* dpy, win* w, int a) {
    // if (w->t) {
    //     XSetWindowBackground(dpy, w->t, (a ? TITLEBAR_ACTIVE_COLOR : TITLEBAR_INACTIVE_COLOR));
    // }
    XSetWindowBorder(dpy, w->window, (a ? BORDER_ACTIVE_COLOR : BORDER_INACTIVE_COLOR));
    Window temp = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), w->x, w->y - TITLEBAR_HEIGHT, w->w + BORDER_WIDTH * 2, TITLEBAR_HEIGHT, 0, (a ? TITLEBAR_ACTIVE_COLOR : TITLEBAR_INACTIVE_COLOR), (a ? TITLEBAR_ACTIVE_COLOR : TITLEBAR_INACTIVE_COLOR));
    XSelectInput(dpy, w->t, NoEventMask);
    XSelectInput(dpy, temp, EnterWindowMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    XMapWindow(dpy, temp);
    XUnmapWindow(dpy, w->t);
    XDestroyWindow(dpy, w->t);
    w->t = temp;
    w_arr[0] = w->window;
    w_arr[1] = w->t;
    w_arr[2] = w->s;
    XRestackWindows(dpy, w_arr, 3);
}

void _destroy_decorations(Display* dpy, win* w) {
    XSelectInput(dpy, w->window, NoEventMask);
    XSelectInput(dpy, w->s, NoEventMask);
    XSelectInput(dpy, w->t, NoEventMask);
    XUnmapWindow(dpy, w->s);
    XDestroyWindow(dpy, w->s);
    XUnmapWindow(dpy, w->t);
    XDestroyWindow(dpy, w->t);
    w->s = 0;
    w->t = 0;
 }

/* Keyboard - Mouse Functions */
void close(Display* dpy, XEvent ev, int arg) {
    if (active) {
        _destroy_decorations(dpy, active);
        XKillClient(dpy, active->window); 
    
        if (active->prev == active) { 
            win_list = 0; 
            active = 0;
        } else {
            if (active->next) { active->next->prev = active->prev; }
            if (active->prev) { active->prev->next = active->next; }
            switch_window(dpy, ev, arg);
        }
    }
}

void maximize(Display* dpy, XEvent ev, int arg) {
    if (active) {
        XWindowAttributes attr;
        XGetWindowAttributes(dpy, active->window, &attr);
        if (attr.width == XDisplayWidth(dpy, DefaultScreen(dpy)) && attr.height == XDisplayHeight(dpy, DefaultScreen(dpy))) {
            XMoveResizeWindow(dpy, active->window, active->x, active->y, active->w, active->h);
        } else {
            XMoveResizeWindow(dpy, active->window, -BORDER_WIDTH, -BORDER_WIDTH, XDisplayWidth(dpy, DefaultScreen(dpy)), XDisplayHeight(dpy, DefaultScreen(dpy)));
        }
    }
}

void switch_window(Display* dpy, XEvent ev, int arg) {
    active = active->prev;
    XRaiseWindow(dpy, active->s);
    XRaiseWindow(dpy, active->t);
    XRaiseWindow(dpy, active->window);
    XSetInputFocus(dpy, active->window, RevertToParent, CurrentTime);
}

void move(Display* dpy, XEvent ev, int arg) {
    if (active) {
        XRaiseWindow(dpy, active->window);
        switch(arg) {
            case LEFT:
                active->x -= MOVE_DELTA;
                break;
            case DOWN:
                active->y += MOVE_DELTA;
                break;
            case UP:
                active->y -= MOVE_DELTA;
                break;
            case RIGHT:
                active->x += MOVE_DELTA;
                break;
            default:
                break;
        }
        XMoveResizeWindow(dpy, active->window, active->x, active->y, active->w, active->h);
        XMoveResizeWindow(dpy, active->s, active->x + SHADOW_X, active->y + SHADOW_Y - TITLEBAR_HEIGHT, active->w, active->h + TITLEBAR_HEIGHT);
        XMoveResizeWindow(dpy, active->t, active->x, active->y - TITLEBAR_HEIGHT, active->w + BORDER_WIDTH * 2, TITLEBAR_HEIGHT);
    }
}

void logout(Display* dpy, XEvent ev, int arg) {
    running = 0;
}

void event_handler(Display* dpy, XEvent ev) {
    win *w;
    switch (ev.type) {
        case ConfigureRequest:
            XConfigureWindow(dpy, ev.xconfigurerequest.window, ev.xconfigurerequest.value_mask, &(XWindowChanges) {
                .x = ev.xconfigurerequest.x,
                .y = ev.xconfigurerequest.y,
                .width = ev.xconfigurerequest.width,
                .height = ev.xconfigurerequest.height,
            });
            for (ALL_WINDOWS) {
                if (w->window == ev.xconfigurerequest.window) {
                    w->x = ev.xconfigurerequest.x;
                    w->y = ev.xconfigurerequest.y;
                    w->w = ev.xconfigurerequest.width;
                    w->h = ev.xconfigurerequest.height;

                    //XMoveResizeWindow(dpy, w->window, w->x, w->y, w->w, w->h);
                    XMoveResizeWindow(dpy, w->s, w->x + SHADOW_X, w->y + SHADOW_Y - TITLEBAR_HEIGHT, w->w + BORDER_WIDTH * 2, w->h + BORDER_WIDTH * 2 + TITLEBAR_HEIGHT);
                    XMoveResizeWindow(dpy, w->t, w->x, w->y - TITLEBAR_HEIGHT, w->w + BORDER_WIDTH * 2, TITLEBAR_HEIGHT); 
                }
            }

            
            break;
        case MapRequest:
            if (!(w = (win *) calloc(1, sizeof(win)))) { exit(1); }
            win_size(ev.xmaprequest.window, &(w->x), &(w->y), &(w->w), &(w->h));
            w->window = ev.xmaprequest.window;
            XSelectInput(dpy, w->window, StructureNotifyMask | EnterWindowMask | FocusChangeMask);
            w->s = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), w->x + SHADOW_X, w->y + SHADOW_Y, w->w + BORDER_WIDTH * 2, w->h + TITLEBAR_HEIGHT + BORDER_WIDTH * 2, 0, SHADOW_COLOR, SHADOW_COLOR);
            w->t = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), w->x, w->y, w->w + BORDER_WIDTH * 2, TITLEBAR_HEIGHT, 0, TITLEBAR_INACTIVE_COLOR, TITLEBAR_INACTIVE_COLOR);
            XSelectInput(dpy, w->t, EnterWindowMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
            XMoveResizeWindow(dpy, w->window, w->x, w->y + TITLEBAR_HEIGHT, w->w, w->h);
            w->y += TITLEBAR_HEIGHT;
            active = w;
            if (win_list) {
                win_list->prev->next = w;
                w->prev = win_list->prev;
                win_list->prev = w;
                w->next = win_list;
            } else {
                win_list = w;
                win_list->prev = win_list->next = win_list;
            }
            XSetWindowBorderWidth(dpy, w->window, BORDER_WIDTH);
            XMapWindow(dpy, w->s);
            XMapWindow(dpy, w->t);
            XMapWindow(dpy, w->window);
            XRaiseWindow(dpy, w->window);
            XSetInputFocus(dpy, w->window, RevertToParent, CurrentTime);
            break;
        case KeyPress:
            key_handler(dpy, ev);
            break;
        case EnterNotify:
            for (ALL_WINDOWS) {
                if (w->window == ev.xcrossing.window || w->t == ev.xcrossing.window) {
                    XSetInputFocus(dpy, w->window, RevertToParent, CurrentTime);
                    active = w;
                }
            }
            break;
        case DestroyNotify:
            for (ALL_WINDOWS) {
                if (w->window == ev.xdestroywindow.window) {
                    _destroy_decorations(dpy, w);
    
                    if (!win_list || !w) { return; }
                    if (w->prev == w) { 
                        win_list = 0; 
                        active = 0;
                        break;
                    }
                    if (win_list == w) { win_list = w->next; }
                    if (w->next) { w->next->prev = w->prev; }
                    if (w->prev) { w->prev->next = w->next; }
                    
                    switch_window(dpy, ev, 0);
                    break;
                }
            }
            break;
        case UnmapNotify:
            for (ALL_WINDOWS) {
                if (w->window == ev.xunmap.window) {
                    XSelectInput(dpy, w->window, NoEventMask);
                    XUnmapWindow(dpy, w->window);
                    _destroy_decorations(dpy, w);
                }
            }
            break;
        case FocusIn:
            for (ALL_WINDOWS) {
                if (w->window == ev.xfocus.window) {
                    _focus(dpy, w, ACTIVE);
                }
            }
            break;
        case FocusOut:
            for (ALL_WINDOWS) {
                if (w->window == ev.xfocus.window) {
                    _focus(dpy, w, INACTIVE);
                }
            }
            break;
        case ButtonPress:
            break;
        case ButtonRelease:
            break;
        case MotionNotify:
            break;
        default:
            break;
    }
}

int main(void)
{


    if(!(dpy = XOpenDisplay(0x0))) return 1;

    lll("===");
    lll("session");
    XSetErrorHandler(error_handler);

    XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureRedirectMask);
    XGrabButton(dpy, 1, MOVE_KEY, DefaultRootWindow(dpy), True, ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
    XGrabButton(dpy, 3, MOVE_KEY, DefaultRootWindow(dpy), True, ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
    key_init(dpy);

    XDefineCursor(dpy, DefaultRootWindow(dpy), XCreateFontCursor(dpy, 68));
    start.subwindow = None;
    while(running) {
        XNextEvent(dpy, &ev);

        event_handler(dpy, ev);
        if (ev.type == ButtonPress) {
            start = ev.xbutton;
            if (ev.xbutton.subwindow == None && !(ev.xbutton.state & MOVE_KEY)) { 
                start.subwindow = active->window;
            } 

        } else if (ev.type == MotionNotify && start.subwindow != None) {
            int xdiff = ev.xbutton.x_root - start.x_root;
            int ydiff = ev.xbutton.y_root - start.y_root;

            XMoveResizeWindow(dpy, active->window, 
                active->x + (start.button == 1 ? xdiff : 0), 
                active->y + (start.button == 1 ? ydiff : 0), 
                MAX(1, active->w + (start.button == 3 ? xdiff : 0)), 
                MAX(1, active->h + (start.button == 3 ? ydiff : 0)));
            XMoveResizeWindow(dpy, active->s, 
                active->x + SHADOW_X + (start.button == 1 ? xdiff : 0), 
                active->y + SHADOW_Y - TITLEBAR_HEIGHT + (start.button == 1 ? ydiff : 0), 
                MAX(1, active->w + BORDER_WIDTH * 2 + (start.button == 3 ? xdiff : 0)), 
                MAX(1, active->h + TITLEBAR_HEIGHT + BORDER_WIDTH * 2 + (start.button == 3 ? ydiff : 0)));
            XMoveResizeWindow(dpy, active->t, 
                active->x + (start.button == 1 ? xdiff : 0), 
                active->y - TITLEBAR_HEIGHT + (start.button == 1 ? ydiff : 0), 
                MAX(1, active->w + BORDER_WIDTH * 2 + (start.button == 3 ? xdiff : 0)), 
                TITLEBAR_HEIGHT);

        }
        else if(ev.type == ButtonRelease) {
            start.subwindow = None;
            if (active) {
                win_size(active->window, &(active->x), &(active->y), &(active->w), &(active->h));
            }
        }
    }
}
