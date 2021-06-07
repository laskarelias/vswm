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
    this->t = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), x, y, w, h + TITLEBAR_HEIGHT, 0, TITLEBAR_ACTIVE, TITLEBAR_ACTIVE);
    XSelectInput(dpy, t, EnterWindowMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ExposureMask);
    XMapWindow(dpy, t);
    b.push_back(button(dpy, this, 0, close));
    b.push_back(button(dpy, this, 1, maximize));
    XSetWindowBorderWidth(dpy, wid, 0);
    XSetWindowBorder(dpy, t, BORDER_ACTIVE);
    XSetWindowBorderWidth(dpy, t, BORDER_WIDTH);
    XReparentWindow(dpy, wid, t, 0, TITLEBAR_HEIGHT);
}

void vswin::destroy(Display* dpy) {
    winlist.remove(*this);
    XUnmapWindow(dpy, t);
    XSelectInput(dpy, wid, NoEventMask);
    XSelectInput(dpy, t, NoEventMask);
    XKillClient(dpy, wid);
    if (winlist.size() > 0) { 
        winlist.back().focus(dpy); 
    } else {
        active = nullptr;
    }
    return;
}

void vswin::focus(Display* dpy) {
    active = this;
    XSetWindowBorder(dpy, t, BORDER_ACTIVE);
    XSetWindowBackground(dpy, t, TITLEBAR_ACTIVE);
    XClearWindow(dpy, t);
    return;
}

void vswin::unfocus(Display* dpy) {
    this->a = false;
    XSetWindowBorder(dpy, t, BORDER_INACTIVE);
    XSetWindowBackground(dpy, t, TITLEBAR_INACTIVE);
    XClearWindow(dpy, t);
    return;
}

void vswin::move(Display* dpy, int btn, int x, int y) {
    lll("in move");
    switch (btn) {
        case 1:
            XMoveWindow(dpy, t, this->x + x, this->y + y);
            break;
        case 3:
            XResizeWindow(dpy, t, this->w + x, this->h + y);
            XResizeWindow(dpy, wid, this->w + x, this->h + y - TITLEBAR_HEIGHT);
            break;
        default:
            break;
    }
}

button::button(Display* dpy, vswin* p, int i, void (* function)(Display* dpy, XEvent ev, int arg)) {
    XGCValues gcv;
    this->bid = XCreateSimpleWindow(dpy, p->t, 2 + i * TITLEBAR_HEIGHT, 2, TITLEBAR_HEIGHT - 6, TITLEBAR_HEIGHT - 6, 0, 0xFF0000, 0x000000);
    XMapWindow(dpy, bid);
    this->bgc = XCreateGC(dpy, bid, 0, 0);    
    this->decorate(dpy);
    this->function = function; 
}

void button::decorate(Display* dpy) {
    XGCValues gcv;
    XClearWindow(dpy, bid);
    XSetForeground(dpy, bgc, 0xFFFFFF);
    XSetFillStyle(dpy, bgc, FillSolid);
    XFillRectangle(dpy, bid, bgc, 0, 0, TITLEBAR_HEIGHT-7, TITLEBAR_HEIGHT-7);
    XSetForeground(dpy, bgc, 0x828282);
    XFillRectangle(dpy, bid, bgc, 1, 1, TITLEBAR_HEIGHT-8, TITLEBAR_HEIGHT-8);
    XSetForeground(dpy, bgc, 0xC3C3C3);
    XFillRectangle(dpy, bid, bgc, 1, 1, TITLEBAR_HEIGHT-9, TITLEBAR_HEIGHT-9);
}

void close(Display* dpy, XEvent ev, int arg) {
    if (active != nullptr) {
        active->destroy(dpy);
    }
    return;
}

void maximize(Display* dpy, XEvent ev, int arg) {
    if (active != nullptr) {
        XWindowAttributes attr;
        XGetWindowAttributes(dpy, active->t, &attr);
        if (attr.width == XDisplayWidth(dpy, DefaultScreen(dpy)) && attr.height == XDisplayHeight(dpy, DefaultScreen(dpy)) && attr.x == -BORDER_WIDTH && attr.y == -BORDER_WIDTH) { 
            XMoveResizeWindow(dpy, active->t, active->x, active->y, active->w, active->h); 
            XResizeWindow(dpy, active->wid, active->w, active->h - TITLEBAR_HEIGHT); 
        }
        else {
            XMoveResizeWindow(dpy, active->t, 0, 0, XDisplayWidth(dpy, DefaultScreen(dpy)), XDisplayHeight(dpy, DefaultScreen(dpy)));
            XResizeWindow(dpy, active->wid, XDisplayWidth(dpy, DefaultScreen(dpy)), XDisplayHeight(dpy, DefaultScreen(dpy)) - TITLEBAR_HEIGHT);
            
        }
    }
    return;
}

void info(Display* dpy, XEvent ev, int arg) {
    if (winlist.size() == 0) {return;}
    for (auto &i : winlist) {
        std::cout << i.wid << ": " << i.w << "x" << i.h << "+" << i.x << "+" << i.y << "!" << i.t << std::endl;
    }
    std::cout << "Active: " << active->wid << std::endl;
    return;
}