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

static win *win_list = {0};
static win *active = {0};

void _draw_decorations() {}

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
        XUnmapWindow(dpy, active->s);
        XDestroyWindow(dpy, active->s);
        XUnmapWindow(dpy, active->t);
        XDestroyWindow(dpy, active->t);
        active->s = 0;
        active->t = 0;
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
        if (attr.width == XDisplayWidth(dpy, DefaultScreen(dpy)) - 2 * BORDER_WIDTH && attr.height == XDisplayHeight(dpy, DefaultScreen(dpy)) - 2 * BORDER_WIDTH) {
            XMoveResizeWindow(dpy, active->window, XDisplayWidth(dpy, DefaultScreen(dpy)) / 3, XDisplayHeight(dpy, DefaultScreen(dpy)) / 3, (XDisplayWidth(dpy, DefaultScreen(dpy)) - 2 * BORDER_WIDTH) / 3, (XDisplayHeight(dpy, DefaultScreen(dpy)) - 2 * BORDER_WIDTH) / 3);
        } else {
            XMoveResizeWindow(dpy, active->window, 0, 0, XDisplayWidth(dpy, DefaultScreen(dpy)) - 2 * BORDER_WIDTH, XDisplayHeight(dpy, DefaultScreen(dpy)) - 2 * BORDER_WIDTH);
        }
    }
}

void switch_window(Display* dpy, XEvent ev, int arg) {
    XLowerWindow(dpy, active->window);
    XRaiseWindow(dpy, active->next->window);
    XSetInputFocus(dpy, active->prev->window, RevertToParent, CurrentTime);
}

void move(Display* dpy, XEvent ev, int arg) {
    if (active) {
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
        }
        XMoveResizeWindow(dpy, active->window, active->x, active->y + TITLEBAR_HEIGHT, active->w, active->h);
        XMoveResizeWindow(dpy, active->s, active->x + SHADOW_X, active->y + SHADOW_Y, active->w, active->h);
        XMoveResizeWindow(dpy, active->t, active->x, active->y, active->w + BORDER_WIDTH * 2, TITLEBAR_HEIGHT);
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
            lll("maprequest");
            if (!(w = (win *) calloc(1, sizeof(win)))) { exit(1); }
            win_size(ev.xmaprequest.window, &(w->x), &(w->y), &(w->w), &(w->h));
            XSelectInput(dpy, ev.xmaprequest.window, StructureNotifyMask | EnterWindowMask | FocusChangeMask);
            w->s = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), w->x + SHADOW_X, w->y + SHADOW_Y, w->w, w->h + TITLEBAR_HEIGHT, 0, SHADOW_COLOR, SHADOW_COLOR);
            w->t = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), w->x, w->y, w->w + BORDER_WIDTH * 2, TITLEBAR_HEIGHT, 0, TITLEBAR_COLOR, TITLEBAR_COLOR);
            XMoveResizeWindow(dpy, ev.xmaprequest.window, w->x, w->y + TITLEBAR_HEIGHT, w->w, w->h);
            w->window = ev.xmaprequest.window;
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
            XSetWindowBorderWidth(dpy, ev.xmaprequest.window, BORDER_WIDTH);
            XSetWindowBorder(dpy, ev.xmaprequest.window, INACTIVE_COLOR);
            XMapWindow(dpy, w->s);
            XMapWindow(dpy, w->t);
            XSelectInput(dpy, w->s, NoEventMask);
            XSelectInput(dpy, w->t, NoEventMask);
            XMapWindow(dpy, ev.xmaprequest.window);
            XRaiseWindow(dpy, ev.xmaprequest.window);
            XSetInputFocus(dpy, ev.xmaprequest.window, RevertToParent, CurrentTime);
            break;
        case KeyPress:
            key_handler(dpy, ev);
            break;
        case EnterNotify:
            for (ALL_WINDOWS) {
                if (w->window == ev.xcrossing.window) {
                    XSetInputFocus(dpy, ev.xcrossing.window, RevertToParent, CurrentTime);
                    active = w;
                }
            }
            break;
        case DestroyNotify:
            lll("destroy notify");
            for (ALL_WINDOWS) {
                lll("loop");
                if (w->window == ev.xdestroywindow.window) {
                    lll("found!");
                    XSelectInput(dpy, ev.xdestroywindow.window, NoEventMask);
                    if (!win_list || !w) { return; }
                    if (w->prev == w) { 
                        lll("no_win");
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
    XButtonEvent start;
    static XEvent ev;

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

        if (ev.type == ButtonPress && ev.xbutton.subwindow != None) {
            start = ev.xbutton;
            active->window = start.subwindow;
        } else if (ev.type == MotionNotify && start.subwindow != None) {
            int xdiff = ev.xbutton.x_root - start.x_root;
            int ydiff = ev.xbutton.y_root - start.y_root;

            // active->x += (start.button == 1 ? xdiff : 0);
            // active->y += (start.button == 1 ? ydiff : 0);
            // active->w = MAX(1, active->w + (start.button == 3 ? xdiff : 0));
            // active->h = MAX(1, active->h + (start.button == 3 ? ydiff : 0));

            active->x += (start.button == 1 ? 1 : 0);
            active->y += (start.button == 1 ? 1 : 0);
            active->w = MAX(1, active->w + (start.button == 3 ? 1 : 0));
            active->h = MAX(1, active->h + (start.button == 3 ? 1 : 0));
            
            XMoveResizeWindow(dpy, active->window, active->x, active->y, active->w, active->h);
            XMoveResizeWindow(dpy, active->s, active->x + SHADOW_X, active->y + SHADOW_Y - TITLEBAR_HEIGHT, active->w, active->h + TITLEBAR_HEIGHT);
            XMoveResizeWindow(dpy, active->t, active->x, active->y - TITLEBAR_HEIGHT, active->w + BORDER_WIDTH * 2, TITLEBAR_HEIGHT);

        }
        else if(ev.type == ButtonRelease)
            start.subwindow = None;
    }
}

