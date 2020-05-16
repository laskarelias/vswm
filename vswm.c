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

void lll(char msg[]){
    FILE * fp;
    fp = fopen("log.txt", "a");
    fprintf(fp, "%s \n", msg);
    fclose(fp);
}

int main(void)
{
    Display * dpy;
    XWindowAttributes attr;
    XButtonEvent start;
    XEvent ev;

    int wx, wy;
    unsigned int ww, wh;

    if(!(dpy = XOpenDisplay(0x0))) return 1;

    lll("session");

    XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureRedirectMask | EnterWindowMask | LeaveWindowMask);
    XGrabButton(dpy, 1, Mod1Mask, DefaultRootWindow(dpy), True,
            ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
    XGrabButton(dpy, 3, Mod1Mask, DefaultRootWindow(dpy), True,
            ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
    XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("c")), Mod1Mask, DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);

    start.subwindow = None;
    for(;;)
    {
        XNextEvent(dpy, &ev);
        if(ev.type == ConfigureRequest) {
            lll("got configurerequest");
            XConfigureWindow(dpy, ev.xconfigurerequest.window, ev.xconfigurerequest.value_mask, &(XWindowChanges) {
                                .x = ev.xconfigurerequest.x,
                                .y = ev.xconfigurerequest.y,
                                .width = ev.xconfigurerequest.width,
                                .height = ev.xconfigurerequest.height,
                                .border_width = BORDER_WIDTH
                             });
            XSetWindowBorder(dpy, ev.xconfigurerequest.window, BORDER_COLOR);
        }
        if(ev.type == MapRequest) {
            lll("got maprequest");
            wx = wy = 0;
            ww = wh = 0;
            win_size(ev.xmaprequest.window, &wx, &wy, &ww, &wh);
            XMoveResizeWindow(dpy, ev.xmaprequest.window, wx, wy, ww, wh);
            XSetWindowBorderWidth(dpy, ev.xmaprequest.window, BORDER_WIDTH);
            XSetWindowBorder(dpy, ev.xmaprequest.window, BORDER_COLOR);
            XMapWindow(dpy, ev.xmaprequest.window);
        }

//       if (ev.type == DestroyNotify) {
//           lll("got destroynotify");
//           XGrabServer(dpy);
//           XSetCloseDownMode(dpy, DestroyAll);
//           XKillClient(dpy, ev.xdestroywindow.window);
//           XUngrabServer(dpy);
//       }
        if (ev.type == KeyPress) {
            lll("got keypress");
            XKillClient(dpy, ev.xkey.subwindow);
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
