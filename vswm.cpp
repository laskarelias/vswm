/* TinyWM is written by Nick Welch <nick@incise.org> in 2005 & 2011.
 *
 * This software is in the public domain
 * and is provided AS IS, with NO WARRANTY. */

#include <iostream>
#include <fstream>
#include <X11/Xlib.h>
#include <list>
#include "helpers.h"
#include "config.h"

std::ofstream log;

static Display* dpy;
static XEvent ev;
static void (*handler[LASTEvent])(Display*, XEvent) = { nullptr };
vswin* active;

std::list <vswin> winlist;

void lll(std::string s) {
    log.open("log.txt", std::ios_base::app);
    log << s << std::endl;
    log.close();
}

int error_handler(Display* dpy, XErrorEvent* ev) {
    return 0;
}

void init_handlers() {
    handler[MapRequest]       = mapreq;
    handler[ConfigureRequest] = configurereq;
    handler[DestroyNotify]    = destroynot;
    handler[EnterNotify]      = enternot;
    handler[FocusIn]          = focusin;
    handler[FocusOut]         = focusout;
    handler[KeyPress]         = key_handler;
}

void init_keys(Display* dpy) {
    for (int i = 0; i < sizeof(keys) / sizeof(*keys); i++) { XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym(keys[i].key)), keys[i].modifiers, DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync); }
}

void key_handler(Display* dpy, XEvent ev) {
    for (int i = 0; i < sizeof(keys) / sizeof(*keys); i++) {
        if ((keys[i].modifiers == ev.xkey.state) && (XKeysymToKeycode(dpy, XStringToKeysym(keys[i].key)) == ev.xkey.keycode)) { keys[i].function(dpy, ev, keys[i].arg); }
    }
}

void configurereq(Display* dpy, XEvent ev) {
    XWindowChanges wc;
    wc.x = ev.xconfigurerequest.x;
    wc.y = ev.xconfigurerequest.y;
    wc.width = ev.xconfigurerequest.width;
    wc.height = ev.xconfigurerequest.height;
    XConfigureWindow(dpy, ev.xconfigurerequest.window, ev.xconfigurerequest.value_mask, &wc);
}

void destroynot(Display* dpy, XEvent ev) {
    for (auto &i : winlist) {
        if (ev.xdestroywindow.window == i.wid) {
            i.destroy(dpy);
            winlist.remove(i);
            return;
        }
    }
    return;
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
    // XSetWindowBorderWidth(dpy, ev.xmaprequest.window, 4);
    XMapWindow(dpy, ev.xmaprequest.window);
    //xgetgeometry trolling
    if (!found) { lll("NOT found window"); winlist.push_back(vswin(dpy, ev.xmaprequest.window, attr.x, attr.y, attr.width, attr.height)); lll("added new");}
    XSetInputFocus(dpy, ev.xmaprequest.window, RevertToParent, CurrentTime);
}

void enternot(Display* dpy, XEvent ev) {
    for (auto &i : winlist) {
        if ((i.wid == ev.xcrossing.window) | (i.t == ev.xcrossing.window)) {
            XSetInputFocus(dpy, i.wid, RevertToParent, CurrentTime);
        }
    }
    
}

void focusin(Display* dpy, XEvent ev) {
    for (auto &i : winlist) {
        if (i.wid == ev.xfocus.window) {
            lll("Focused window");
            i.focus(dpy);
        }
    }
}

void focusout(Display* dpy, XEvent ev) {
    for (auto &i : winlist) {
        if (i.wid == ev.xfocus.window) {
            lll("unfocused window");
            i.unfocus(dpy);
        }
    }
}

int main(void) {
    
    if(!(dpy = XOpenDisplay(0x0))) return 1;
    XSetErrorHandler(error_handler);
    XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureRedirectMask | PropertyChangeMask);
    init_handlers();
    init_keys(dpy);
    for(;;)
    {
        XNextEvent(dpy, &ev);
        lll("ev");
        XGrabServer(dpy);
        if (handler[ev.type] != nullptr) { handler[ev.type](dpy, ev); }
        XUngrabServer(dpy);
    }
}
