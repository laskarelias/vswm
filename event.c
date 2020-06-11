#include <X11/Xlib.h>

#include "vswm.h"
#include "config.h"

event e[];

void event_handler(Display* dpy, XEvent ev) {
    e[ev.type].function(Display* dpy, XEvent ev);
}

void map_req(Display* dpy, XEvent ev) {
    Window window = ev.xmaprequest.window;
    XSelectInput(dpy, window, StructureNotifyMask | EnterWIndowMask | FocusChangeMask);
    XMoveResizeWindow(dpy, window, 200, 200, 400, 400);
    XSetWindowBorderWidth(dpy, window, BORDER_WIDTH);
    XSetWindowBorder(dpy, window, INACTIVE_COLOR);
    XMapWindow(dpy, window);
}

void config_req(Display* dpy, XEvent ev) {
    Window window = ev.xconfigurerequest.window
    XConfigureWindow(dpy, ev.xconfigurerequest.window, ev.xconfigurerequest.value_mask, &(XWindowChanges) {
        .x = ev.xconfigurerequest.x,
        .y = ev.xconfigurerequest.y,
        .width = ev.xconfigurerequest.width,
        .height = ev.xconfigurerequest.height,
        .border_width = BORDER_WIDTH
    } );
}

void key_press(Display* dpy, XEvent ev) {
    key_handler(dpy, ev);
}

void enter_not(Display* dpy, XEvent ev) {
    XSetInputFocus(dpy, ev.xcrossing.window, RevertToParent, CurrentTime);
}

void destroy_not(Display* dpy, XEvent ev) {
    XSelectInput(dpy, ev.xdestroywindow.window, NoEventMask);
}

void unmap_not(Display* dpy, XEvent ev) {
    XSelectInput(dpy, ev.xunmap.window, NoEventMask);
    XUnmapWindow(dpy, ev.xunmap.window);
}

void focus_in(Display* dpy, XEvent ev) {
    XSetWindowBorder(dpy, ev.xfocus.window, ACTIVE_COLOR);
}

void focus_out(Display* dpy, XEvent ev) {
    XSetWindowBorder(dpy, ev.xfocus.window, INACTIVE_COLOR);
}

