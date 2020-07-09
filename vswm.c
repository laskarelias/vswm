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
#define DISPLAY_WIDTH XDisplayWidth(dpy, DefaultScreen(dpy))
#define DISPLAY_HEIGHT XDisplayHeight(dpy, DefaultScreen(dpy))
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

/* Functions */
void close(Display* dpy, XEvent ev, int arg) {
    if (active) {
        XSelectInput(dpy, active->window, NoEventMask);
        XKillClient(dpy, active->window); 
//        active = 0;
//      
        if (active->prev == active) { 
            win_list = 0; 
            active = 0;
        } else {
            if (active->next) { active->next->prev = active->prev; }
            if (active->prev) { active->prev->next = active->next; }
            active = active->prev;
            XRaiseWindow(dpy, active->window);
            XSetInputFocus(dpy, active->window, RevertToParent, CurrentTime);
        }
    }
}

void maximize(Display* dpy, XEvent ev, int arg) {
    if (active) {
        XWindowAttributes attr;
        XGetWindowAttributes(dpy, active->window, &attr);
        if (attr.width == DISPLAY_WIDTH - 2 * BORDER_WIDTH && attr.height == DISPLAY_HEIGHT - 2 * BORDER_WIDTH) {
            XMoveResizeWindow(dpy, active->window, XDisplayWidth(dpy, DefaultScreen(dpy)) / 3, DISPLAY_HEIGHT / 3, (DISPLAY_WIDTH - 2 * BORDER_WIDTH) / 3, (DISPLAY_HEIGHT - 2 * BORDER_WIDTH) / 3);
        } else {
            XMoveResizeWindow(dpy, active->window, 0, 0, DISPLAY_WIDTH - 2 * BORDER_WIDTH, DISPLAY_HEIGHT - 2 * BORDER_WIDTH);
        }
    }
}

void switch_window(Display* dpy, XEvent ev, int arg) {
    if(active) {
        XLowerWindow(dpy, active->window);
        XSetInputFocus(dpy, active->next->window, RevertToParent, CurrentTime);
    }
}

void move(Display* dpy, XEvent ev, int arg) {
    if (active) {
        XWindowAttributes attr;
        XGetWindowAttributes(dpy, active->window, &attr);
        switch(arg) {
            case LEFT:
                XMoveResizeWindow(dpy, active->window, attr.x - MOVE_DELTA, attr.y, attr.width, attr.height);
                break;
            case DOWN:
                XMoveResizeWindow(dpy, active->window, attr.x, attr.y + MOVE_DELTA, attr.width, attr.height);
                break;
            case UP:
                XMoveResizeWindow(dpy, active->window, attr.x, attr.y - MOVE_DELTA, attr.width, attr.height);
                break;
            case RIGHT:
                XMoveResizeWindow(dpy, active->window, attr.x + MOVE_DELTA, attr.y, attr.width, attr.height);
                break;
        }
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
            break;
        case MapRequest:
            if (!(w = (win *) calloc(1, sizeof(win)))) { exit(1); }
            w->window = ev.xmaprequest.window;
            //win_size(w->window, &(w->x), &(w->y), &(w->w), &(w->h));
            if (!active) {
                w->x = 0;
                w->y = 0;
                w->w = DISPLAY_WIDTH - 2 * BORDER_WIDTH;
                w->h = DISPLAY_HEIGHT - 2 * BORDER_WIDTH;
                if (w->w > w->h) { w->size = wide; }
                else { w->size = tall; }
            } else {
                if (active->size == wide) {
                    w->x = active->x + (active->w / 2);
                    w->y = active->y;
                    w->w = active->w / 2;
                    w->h = active->h;
                } else {
                    w->x = active->x;
                    w->y = active->y + (active->h / 2);
                    w->w = active->w;
                    w->h = active->h / 2;
                }
            }
            XSelectInput(dpy, w->window, StructureNotifyMask | EnterWindowMask | FocusChangeMask);
            XMoveResizeWindow(dpy, w->window, w->x, w->y, w->w, w->h);
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
            XSetWindowBorder(dpy, w->window, INACTIVE_COLOR);
            XMapWindow(dpy, w->window);
            XRaiseWindow(dpy, w->window);
            XSetInputFocus(dpy, w->window, RevertToParent, CurrentTime);
            break;
        case KeyPress:
            key_handler(dpy, ev);
            break;
        case EnterNotify:
            for (ALL_WINDOWS) {
                if (w->window == ev.xcrossing.window) {
                    XSetInputFocus(dpy, w->window, RevertToParent, CurrentTime);
                    active = w;
                }
            }
            break;
        case DestroyNotify:
            for (ALL_WINDOWS) {
                if (w->window == ev.xdestroywindow.window) {
                    XSelectInput(dpy, w->window, NoEventMask);
                    if (!win_list || !w) { return; }
                    if (w->prev == w) { 
                        win_list = 0; 
                        active = 0;
                        break;
                    }
                    if (win_list == w) { win_list = w->next; }
                    if (w->next) { w->next->prev = w->prev; }
                    if (w->prev) { w->prev->next = w->next; }
                    active = w->prev;
                    break;
                }
            }
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

    if(!(dpy = XOpenDisplay(0x0))) return 1;

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
        if (ev.type == ButtonPress && ev.xbutton.subwindow != None) {
            XGetWindowAttributes(dpy, ev.xbutton.subwindow, &attr);
            start = ev.xbutton;
        } else if (ev.type == MotionNotify && start.subwindow != None) {
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

