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
    this->t = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), x, y, 
        w + INNER_BORDER_WIDTH * 2 + FRAME_WIDTH * 2, 
        h + TITLEBAR_HEIGHT + INNER_BORDER_WIDTH * 2 + FRAME_WIDTH * 2, 
        OUTER_BORDER_WIDTH, TITLEBAR_ACTIVE, TITLEBAR_ACTIVE);
    XSelectInput(dpy, t, EnterWindowMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ExposureMask);
    XMapWindow(dpy, t);
    unsigned int i = 0;
    switch (TITLEBAR_BUTTONS) {
        case left:
            for (i = 0; i < sizeof(title_buttons) / sizeof(*title_buttons); i++) { 
                b.push_back(button(dpy, this, 
                    FRAME_WIDTH + i * (BUTTON_WIDTH + BUTTON_SPACING), 
                    (TITLEBAR_POSITION == bottom) ? 
                        h + FRAME_WIDTH + 2 * INNER_BORDER_WIDTH + TITLEBAR_HEIGHT - BUTTON_HEIGHT - 2 * BUTTON_BORDER_WIDTH: 
                        FRAME_WIDTH, 
                    BUTTON_WIDTH, BUTTON_HEIGHT, (int)i, (char *)(title_buttons[i].text), title_buttons[i].function)); 
            }
            break;
        case right:
            for (i = 0; i < sizeof(title_buttons) / sizeof(*title_buttons); i++) { 
                b.push_back(button(dpy, this, 
                    w + FRAME_WIDTH - BUTTON_WIDTH + 2 * INNER_BORDER_WIDTH - 2 * BUTTON_BORDER_WIDTH - i * (BUTTON_WIDTH + BUTTON_SPACING),
                    (TITLEBAR_POSITION == bottom) ? 
                        h + FRAME_WIDTH + 2 * INNER_BORDER_WIDTH + TITLEBAR_HEIGHT - BUTTON_HEIGHT - 2 * BUTTON_BORDER_WIDTH: 
                        FRAME_WIDTH, 
                    BUTTON_WIDTH, BUTTON_HEIGHT, (int)i, (char *)(title_buttons[i].text), title_buttons[i].function)); 
            }
            break;
        default:
            break;
    }
    if (TITLEBAR_NAME) { 
        switch (TITLEBAR_BUTTONS) {
            case left:
                b.push_back(button(dpy, this, 
                    FRAME_WIDTH + i * (BUTTON_WIDTH + BUTTON_SPACING), 
                    (TITLEBAR_POSITION == bottom) ? 
                        h + FRAME_WIDTH + 2 * INNER_BORDER_WIDTH + TITLEBAR_HEIGHT - BUTTON_HEIGHT - 2 * BUTTON_BORDER_WIDTH : 
                        FRAME_WIDTH,
                    w - i * (BUTTON_WIDTH + BUTTON_SPACING) + (INNER_BORDER_WIDTH * 2), 
                    BUTTON_HEIGHT + 2 * BUTTON_BORDER_WIDTH, 
                    (int)i, "", nothing)); 
                break;
            case right:
                b.push_back(button(dpy, this, 
                    FRAME_WIDTH, 
                    (TITLEBAR_POSITION == bottom) ? 
                        h + FRAME_WIDTH + 2 * INNER_BORDER_WIDTH + TITLEBAR_HEIGHT - BUTTON_HEIGHT - 2 * BUTTON_BORDER_WIDTH : 
                        FRAME_WIDTH, 
                    w - i * (BUTTON_WIDTH + BUTTON_SPACING) + (2 * INNER_BORDER_WIDTH), 
                    BUTTON_HEIGHT + 2 * BUTTON_BORDER_WIDTH, 
                    (int)i, "", nothing)); 
                break;
            default:
                b.push_back(button(dpy, this, 
                    FRAME_WIDTH + i * (BUTTON_WIDTH + BUTTON_SPACING), 
                    (TITLEBAR_POSITION == bottom) ? 
                        h + FRAME_WIDTH + 2 * INNER_BORDER_WIDTH + TITLEBAR_HEIGHT - BUTTON_HEIGHT - 2 * BUTTON_BORDER_WIDTH: 
                        FRAME_WIDTH, 
                    w - i * (BUTTON_WIDTH + BUTTON_SPACING) + (2 * INNER_BORDER_WIDTH), 
                    BUTTON_HEIGHT + 2 * BUTTON_BORDER_WIDTH, 
                    (int)i, "", nothing)); 
                break;
        }
        XSetWindowBorderWidth(dpy, b.back().bid, 0);
        XSetWindowBackground(dpy, b.back().bid, TITLEBAR_ACTIVE);
        XSetForeground(dpy, b.back().bgc, TEXT_ACTIVE);
        XSetBackground(dpy, b.back().bgc, TITLEBAR_ACTIVE);
        b.back().t = true;
    }
    title(dpy);
    XSetWindowBorder(dpy, wid, INNER_BORDER_ACTIVE);
    XSetWindowBorderWidth(dpy, wid, INNER_BORDER_WIDTH);
    XSetWindowBorder(dpy, t, BORDER_ACTIVE);
    XReparentWindow(dpy, wid, t, 
        FRAME_WIDTH, 
        (TITLEBAR_POSITION == bottom) ? 
            FRAME_WIDTH : 
            TITLEBAR_HEIGHT + FRAME_WIDTH);
}

void vswin::destroy(Display* dpy) {
    XGrabServer(dpy);
    winlist.remove(*this);
    XSelectInput(dpy, wid, NoEventMask);
    XSelectInput(dpy, t, NoEventMask);
    XUnmapWindow(dpy, t);
    XKillClient(dpy, wid);
    if (winlist.size() > 0) { 
        winlist.back().focus(dpy); 
    } else {
        active = nullptr;
    }
    XUngrabServer(dpy);
    return;
}

void vswin::unmap(Display* dpy) {
    XUnmapWindow(dpy, t);
    return;
}

void vswin::focus(Display* dpy) {
    active = this;
    XSetWindowBorder(dpy, t, BORDER_ACTIVE);
    XSetWindowBorder(dpy, wid, INNER_BORDER_ACTIVE);
    XSetWindowBackground(dpy, t, TITLEBAR_ACTIVE);
    if (TITLEBAR_BUTTONS) {
        for (auto &i : b) {
            XSetWindowBackground(dpy, i.bid, BUTTON_ACTIVE);
            XSetWindowBorder(dpy, i.bid, BUTTON_BORDER_ACTIVE);
            XSetBackground(dpy, i.bgc, BUTTON_ACTIVE);
            XSetForeground(dpy, i.bgc, TEXT_ACTIVE); 
        } 
    }
    if (TITLEBAR_NAME) { 
        XSetWindowBackground(dpy, b.back().bid, TITLEBAR_ACTIVE);
        XSetBackground(dpy, b.back().bgc, TITLEBAR_ACTIVE);
        XSetForeground(dpy, b.back().bgc, TEXT_ACTIVE); 
    }
    XClearWindow(dpy, t);
    for (auto &j : this->b) {
        j.decorate(dpy);
        j.text(dpy);
    }
    return;
}

void vswin::unfocus(Display* dpy) {
    this->a = false;
    XSetWindowBorder(dpy, t, BORDER_INACTIVE);
    XSetWindowBorder(dpy, wid, INNER_BORDER_INACTIVE);
    XSetWindowBackground(dpy, t, TITLEBAR_INACTIVE);
    if (TITLEBAR_BUTTONS) {
        for (auto &i : b) {
            XSetWindowBackground(dpy, i.bid, BUTTON_INACTIVE);
            XSetWindowBorder(dpy, i.bid, BUTTON_BORDER_INACTIVE);
            XSetBackground(dpy, i.bgc, BUTTON_INACTIVE);
            XSetForeground(dpy, i.bgc, TEXT_INACTIVE); 
        } 
    }
    if (TITLEBAR_NAME) { 
        XSetWindowBackground(dpy, b.back().bid, TITLEBAR_INACTIVE);
        XSetBackground(dpy, b.back().bgc, TITLEBAR_INACTIVE);
        XSetForeground(dpy, b.back().bgc, TEXT_INACTIVE);
    }
    XClearWindow(dpy, t);
    for (auto &j : this->b) {
        j.decorate(dpy);
        j.text(dpy);
    }
    return;
}

void vswin::move(Display* dpy, int btn, int x, int y) {
    lll("in move");
    switch (btn) {
        case 1:
            XMoveWindow(dpy, t, this->x + x, this->y + y);
            break;
        case 3:
            XResizeWindow(dpy, t, 
                this->w + x, 
                this->h + y + TITLEBAR_HEIGHT);
            if (TITLEBAR_NAME > 0) {
                switch (TITLEBAR_NAME) {
                case center:
                    XResizeWindow(dpy, b.back().bid, 
                    b.back().w + x, b.back().h);
                    break;
                case left:
                    XResizeWindow(dpy, b.back().bid, 
                    b.back().w + x, b.back().h);
                    break;
                case right:
                    XResizeWindow(dpy, b.back().bid, 
                    b.back().w + x, b.back().h);
                    break;
                default:
                    break;
                }
            }
            XResizeWindow(dpy, wid, 
                this->w + x - 2 * FRAME_WIDTH - 2 * INNER_BORDER_WIDTH, 
                this->h + y - 2 * FRAME_WIDTH - 2 * INNER_BORDER_WIDTH);
            break;
        default:
            break;
    }
    focus(dpy);
    return;

}

void vswin::title(Display* dpy) {
    XTextProperty xtp;
    if (!(XGetWMName(dpy, wid, &xtp))) { 
        xtp.value = (unsigned char *)"?";
    }
    this->name = (char *)xtp.value;
    if (TITLEBAR_NAME) {
        b.back().txt = name; 
        b.back().text(dpy); 
    }
    return;
}

void vswin::icon(Display* dpy) {
    XIconifyWindow(dpy, this->w, 0);
    return;
}

button::button(Display* dpy, vswin* p, int x, int y, unsigned int w, unsigned int h, int i, char* txt, 
void (* function)(Display* dpy, XEvent ev, int arg)) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->index = i;
    this->t = false;
    this->bid = XCreateSimpleWindow(dpy, p->t, x, y, w, h, BUTTON_BORDER_WIDTH, BUTTON_BORDER_ACTIVE, BUTTON_ACTIVE);
    this->txt = txt;
    XMapWindow(dpy, bid);
    this->bgc = XCreateGC(dpy, bid, 0, 0);
    XSetBackground(dpy, bgc, BUTTON_ACTIVE);
    XSetForeground(dpy, bgc, TEXT_ACTIVE);    
    this->decorate(dpy);
    this->text(dpy);
    this->function = function; 
}

void button::decorate(Display* dpy) {
    XClearWindow(dpy, bid);
    if (t) { return; }
    XSetFillStyle(dpy, bgc, FillSolid);
    // Win98 style
    // XSetForeground(dpy, bgc, 0xFFFFFF);
    // XFillRectangle(dpy, bid, bgc, 0, 0, BUTTON_HEIGHT, BUTTON_HEIGHT);
    // XSetForeground(dpy, bgc, 0x808080);
    // XFillRectangle(dpy, bid, bgc, 1, 1, BUTTON_HEIGHT-1, BUTTON_HEIGHT-1);
    // XSetForeground(dpy, bgc, 0xDFDFDF);
    // XFillRectangle(dpy, bid, bgc, 1, 1, BUTTON_HEIGHT-2, BUTTON_HEIGHT-2);
    // XSetForeground(dpy, bgc, 0xC0C0C0);
    // XFillRectangle(dpy, bid, bgc, 2, 2, BUTTON_HEIGHT-3, BUTTON_HEIGHT-3);
    // return;
}

void button::text(Display* dpy) {
    int x, y, d, asc, desc;
    XCharStruct overall;
    XFontStruct* font = XLoadQueryFont(dpy, TEXT_FONT);
    XSetFillStyle(dpy, this->bgc, FillSolid);
    XSetFont(dpy, bgc, font->fid);
    XTextExtents(font, txt, strlen(txt), &d, &asc, &desc, &overall);
    y = ((this->h + asc - desc) / 2);
    switch (TITLEBAR_NAME) {
        case left:
            x = 3;
            break;
        case center:
            x = (this->w - overall.width) / 2;
            break;
        case right:
            x = (this->w - overall.width) - 3;
            break;
        default:
            x = 3;
            break;
    }
    XClearWindow(dpy, bid);
    decorate(dpy);
    // XSetBackground(dpy, bgc, BUTTON_COLOR);
    // XSetForeground(dpy, this->bgc, TEXT_ACTIVE);
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
        if (attr.width == XDisplayWidth(dpy, DefaultScreen(dpy)) && 
        attr.height == XDisplayHeight(dpy, DefaultScreen(dpy)) && 
        attr.x == -FRAME_WIDTH && 
        attr.y == -FRAME_WIDTH) { 
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
    //     if (attr.width == XDisplayWidth(dpy, DefaultScreen(dpy)) && attr.height == XDisplayHeight(dpy, DefaultScreen(dpy)) && attr.x == -FRAME_WIDTH && attr.y == -FRAME_WIDTH) { 
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

void iconify(Display* dpy, XEvent ev, int arg) {
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
