#include <X11/Xlib.h>
#include <unistd.h>
#include <iostream>

#include "helpers.h"
#include "config.h"

vswin::vswin(Display* dpy, Window wid, int x, int y, unsigned int w, unsigned int h) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->wid = wid;
    this->a = true;
    this->t = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), x, y, w, h + TITLEBAR_HEIGHT, 0, TITLEBAR_ACTIVE, TITLEBAR_ACTIVE);
    XMapWindow(dpy, t);
    XSetWindowBorder(dpy, t, BORDER_ACTIVE);
    XSetWindowBorderWidth(dpy, t, BORDER_WIDTH);
    XReparentWindow(dpy, wid, t, 0, TITLEBAR_HEIGHT);
}

void vswin::focus() {
    a = true;
}

void vswin::unfocus() {
    a = false;
}