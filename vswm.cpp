/* TinyWM is written by Nick Welch <nick@incise.org> in 2005 & 2011.
 *
 * This software is in the public domain
 * and is provided AS IS, with NO WARRANTY. */

#include <iostream>
#include <fstream>
#include <X11/Xlib.h>
#include <list>
#include "helpers.h"

std::ofstream log;

static Display* dpy;
static XEvent ev;
static void (*handler[LASTEvent])(Display*, XEvent) = { nullptr };

std::list <vswin> winlist;

void lll(std::string s) {
    log.open("log.txt", std::ios_base::app);
    log << s << std::endl;
    log.close();
}

void init_handlers() {
    handler[MapRequest]       = mapreq;
    handler[ConfigureRequest] = configurereq;
    handler[EnterNotify]      = enternot;
    handler[FocusIn]          = focusin;
    handler[FocusOut]         = focusout;
}

void configurereq(Display* dpy, XEvent ev) {
    XWindowChanges wc;
    wc.x = ev.xconfigurerequest.x;
    wc.y = ev.xconfigurerequest.y;
    wc.width = ev.xconfigurerequest.width;
    wc.height = ev.xconfigurerequest.height;
    XConfigureWindow(dpy, ev.xconfigurerequest.window, ev.xconfigurerequest.value_mask, &wc);
}

void mapreq(Display* dpy, XEvent ev) {
    lll("mapreq");
    bool found = false;
    XWindowAttributes attr;
    XGetWindowAttributes(dpy, ev.xmaprequest.window, &attr);
    if (winlist.size() > 0) {
        lll("something in winlist");
        for (auto &i : winlist) {
            lll("iterating...");
            if (i.wid == ev.xmaprequest.window) { lll("found window"); found = true; }        
        }
    }
    XSelectInput(dpy, ev.xmaprequest.window, StructureNotifyMask | EnterWindowMask | FocusChangeMask | PropertyChangeMask);
    XSetWindowBorderWidth(dpy, ev.xmaprequest.window, 4);
    XMapWindow(dpy, ev.xmaprequest.window);
    //xgetgeometry trolling
    if (!found) { lll("NOT found window"); winlist.push_back(vswin(ev.xmaprequest.window, attr.x, attr.y, attr.width, attr.height)); lll("added new");}
    XSetInputFocus(dpy, ev.xmaprequest.window, RevertToParent, CurrentTime);
}

void enternot(Display* dpy, XEvent ev) {
    XSetInputFocus(dpy, ev.xcrossing.window, RevertToParent, CurrentTime);
}

void focusin(Display* dpy, XEvent ev) {
    for (auto &i : winlist) {
        if (i.wid == ev.xfocus.window) {
            // VAIOS FOCUS IN
            lll("Focused window");
            XSetWindowBorder(dpy, i.wid, 0x008080);
            i.focus();
        }
    }
}

void focusout(Display* dpy, XEvent ev) {
    for (auto &i : winlist) {
        if (i.wid == ev.xfocus.window) {
            // VAIOS FOCUS IN
            lll("unfocused window");
            XSetWindowBorder(dpy, i.wid, 0x004040);
            i.focus();
        }
    }
}

int main(void) {
    
    if(!(dpy = XOpenDisplay(0x0))) return 1;
    XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureRedirectMask | PropertyChangeMask);
    init_handlers();
    for(;;)
    {
        XNextEvent(dpy, &ev);
        lll("ev");
        XGrabServer(dpy);
        if (handler[ev.type] != nullptr) { handler[ev.type](dpy, ev); }
        XUngrabServer(dpy);
    }
}
