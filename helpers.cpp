#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <unistd.h>
#include <string.h>
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
    this->t = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), x, y, w + INNER_BORDER_WIDTH * 2, h + TITLEBAR_HEIGHT + INNER_BORDER_WIDTH * 2, BORDER_WIDTH, TITLEBAR_ACTIVE, TITLEBAR_ACTIVE);
    XSelectInput(dpy, t, EnterWindowMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ExposureMask);
    XMapWindow(dpy, t);
    unsigned int i = 0;
    if (TITLEBAR_BUTTONS) {
        for (i = 0; i < sizeof(title_buttons) / sizeof(*title_buttons); i++) { b.push_back(button(dpy, this, 2 + i * (BUTTON_WIDTH + BUTTON_SPACING), 2, BUTTON_WIDTH, BUTTON_HEIGHT, (int)i, (char *)(title_buttons[i].text), title_buttons[i].function)); }
    }
    if (TITLEBAR_NAME) { b.push_back(button(dpy, this, 2 + i * (BUTTON_WIDTH + BUTTON_SPACING), 2, w - i * (BUTTON_WIDTH + BUTTON_SPACING) + (INNER_BORDER_WIDTH * 2) - 6, BUTTON_HEIGHT, (int)i, "", nothing)); }
    
    title(dpy);
    XSetWindowBorder(dpy, wid, INNER_BORDER_ACTIVE);
    XSetWindowBorderWidth(dpy, wid, INNER_BORDER_WIDTH);
    XSetWindowBorder(dpy, t, BORDER_ACTIVE);
    XReparentWindow(dpy, wid, t, 0, TITLEBAR_HEIGHT);
}

void vswin::destroy(Display* dpy) {
    XGrabServer(dpy);
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
    XUngrabServer(dpy);
    return;
}

void vswin::focus(Display* dpy) {
    active = this;
    XSetWindowBorder(dpy, t, BORDER_ACTIVE);
    XSetWindowBorder(dpy, wid, INNER_BORDER_ACTIVE);
    XSetWindowBackground(dpy, t, TITLEBAR_ACTIVE);
    XClearWindow(dpy, t);
    return;
}

void vswin::unfocus(Display* dpy) {
    this->a = false;
    XSetWindowBorder(dpy, t, BORDER_INACTIVE);
    XSetWindowBorder(dpy, wid, INNER_BORDER_INACTIVE);
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
            XResizeWindow(dpy, t, this->w + x + INNER_BORDER_WIDTH * 2, this->h + y + INNER_BORDER_WIDTH * 2);
            if (TITLEBAR_NAME) { XResizeWindow(dpy, b.back().bid, this->b.back().w + x, b.back().h); }
            XResizeWindow(dpy, wid, this->w + x, this->h + y - TITLEBAR_HEIGHT);
            break;
        default:
            break;
    }
}

void vswin::title(Display* dpy) {
    XTextProperty xtp;
    if (!(XGetWMName(dpy, wid, &xtp))) { xtp.value = (unsigned char *)"?"; }
    this->name = (char *)xtp.value;
    if (TITLEBAR_NAME) {
        b.back().txt = name; 
        b.back().text(dpy); 
    }
    return;
}

button::button(Display* dpy, vswin* p, int x, int y, unsigned int w, unsigned int h, int i, char* txt, void (* function)(Display* dpy, XEvent ev, int arg)) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->index = i;
    this->bid = XCreateSimpleWindow(dpy, p->t, x, y, w, h, 1, 0x000000, BUTTON_COLOR);
    this->txt = txt;
    XMapWindow(dpy, bid);
    this->bgc = XCreateGC(dpy, bid, 0, 0);    
    this->decorate(dpy);
    this->text(dpy);
    this->function = function; 
}

void button::decorate(Display* dpy) {
    XClearWindow(dpy, bid);
    XSetFillStyle(dpy, bgc, FillSolid);
    // XSetForeground(dpy, bgc, 0xFFFFFF);
    // XSetFillStyle(dpy, bgc, FillSolid);
    // XFillRectangle(dpy, bid, bgc, 0, 0, TITLEBAR_HEIGHT-7, TITLEBAR_HEIGHT-7);
    // XSetForeground(dpy, bgc, 0x808080);
    // XFillRectangle(dpy, bid, bgc, 1, 1, TITLEBAR_HEIGHT-8, TITLEBAR_HEIGHT-8);
    // XSetForeground(dpy, bgc, 0xDFDFDF);
    // XFillRectangle(dpy, bid, bgc, 1, 1, TITLEBAR_HEIGHT-9, TITLEBAR_HEIGHT-9);
    // XSetForeground(dpy, bgc, 0xC0C0C0);
    // XFillRectangle(dpy, bid, bgc, 2, 2, TITLEBAR_HEIGHT-10, TITLEBAR_HEIGHT-10);
}

void button::text(Display* dpy) {
    int x, y, d, asc, desc;
    XCharStruct overall;
    XFontStruct* font = XLoadQueryFont(dpy, TEXT_FONT);
    XSetFillStyle(dpy, this->bgc, FillSolid);
    XSetFont(dpy, bgc, font->fid);
    XTextExtents(font, txt, strlen(txt), &d, &asc, &desc, &overall);
    y = ((this->h + asc - desc) / 2);
    x = 3;
    XClearWindow(dpy, bid);
    XSetBackground(dpy, bgc, BUTTON_COLOR);
    XSetForeground(dpy, this->bgc, TEXT_ACTIVE);
    XDrawImageString(dpy, this->bid, this->bgc, x, y, txt, strlen(txt));
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

void minimize(Display* dpy, XEvent ev, int arg) {
    // if (active != nullptr) {
    //     XWindowAttributes attr;
    //     XGetWindowAttributes(dpy, active->t, &attr);
    //     if (attr.width == XDisplayWidth(dpy, DefaultScreen(dpy)) && attr.height == XDisplayHeight(dpy, DefaultScreen(dpy)) && attr.x == -BORDER_WIDTH && attr.y == -BORDER_WIDTH) { 
    //         XMoveResizeWindow(dpy, active->t, active->x, active->y, active->w, active->h); 
    //         XResizeWindow(dpy, active->wid, active->w, active->h - TITLEBAR_HEIGHT); 
    //     }
    //     else {
    //         XMoveResizeWindow(dpy, active->t, 0, 0, XDisplayWidth(dpy, DefaultScreen(dpy)), XDisplayHeight(dpy, DefaultScreen(dpy)));
    //         XResizeWindow(dpy, active->wid, XDisplayWidth(dpy, DefaultScreen(dpy)), XDisplayHeight(dpy, DefaultScreen(dpy)) - TITLEBAR_HEIGHT);
            
    //     }
    // }
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

void nothing(Display* dpy, XEvent ev, int arg) {
    return;
}
