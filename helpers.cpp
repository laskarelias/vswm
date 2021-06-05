#include <X11/Xlib.h>
#include <unistd.h>
#include <iostream>
#include <list>

#include "helpers.h"
#include "config.h"

extern std::list <vswin> winlist;
extern void lll(std::string s);
extern vswin* active;

vswin::vswin(Display* dpy, Window wid, int x, int y, unsigned int w, unsigned int h) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->wid = wid;
    this->a = true;
    this->t = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), x, y, w, h + TITLEBAR_HEIGHT, 0, TITLEBAR_ACTIVE, TITLEBAR_ACTIVE);
    XSelectInput(dpy, t, EnterWindowMask | FocusChangeMask);
    XMapWindow(dpy, t);
    XSetWindowBorderWidth(dpy, wid, 0);
    XSetWindowBorder(dpy, t, BORDER_ACTIVE);
    XSetWindowBorderWidth(dpy, t, BORDER_WIDTH);
    XReparentWindow(dpy, wid, t, 0, TITLEBAR_HEIGHT);
}

void vswin::destroy(Display* dpy) {
    XUnmapWindow(dpy, t);
    XSelectInput(dpy, wid, NoEventMask);
    XSelectInput(dpy, t, NoEventMask);
    XKillClient(dpy, wid);
}

void vswin::focus(Display* dpy) {
    this->a = true;
    active = this;
    XSetWindowBorder(dpy, t, BORDER_ACTIVE);
    return;
}

void vswin::unfocus(Display* dpy) {
    this->a = false;
    XSetWindowBorder(dpy, t, BORDER_INACTIVE);
    return;
}

void close(Display* dpy, XEvent ev, int arg) {
    if (active != nullptr) {
        active->destroy(dpy);
        winlist.remove(*active);
        active = nullptr;
        if (winlist.size() > 0) {
            winlist.back().focus(dpy);
        }
    }
    return;
}

void info(Display* dpy, XEvent ev, int arg) {
    if (winlist.size() == 0) {return;}
    for (auto &i : winlist) {
        std::cout << i.wid << ": " << i.w << "x" << i.h << "+" << i.x << "+" << i.y << "!" << i.a << std::endl;
    }
    std::cout << "Active: " << active->wid << std::endl;
    return;
}