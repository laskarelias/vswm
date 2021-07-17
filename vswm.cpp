 
/*                         
__   _______      ___ __ ___  
\ \ / / __\ \ /\ / / '_ ` _ \ 
 \ V /\__ \\ V  V /| | | | | |
  \_/ |___/ \_/\_/ |_| |_| |_|
*/

/* VSWM - Very Small Window Manager
 * VSWM - Very Simple Window Manager
 *
 * Thanks - TinyWM
 * Thanks - SOWM
*/

#include <iostream>
#include <fstream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <list>
#include "helpers.h"
#include "config.h"

std::ofstream logfile;

static Display* dpy;
static XEvent ev;
static void (*handler[LASTEvent])(Display*, XEvent) = { nullptr };
static XButtonEvent start;
vswin* active;

std::list <vswin> winlist;

void lll(std::string s) {
    logfile.open("log.txt", std::ios_base::app);
    logfile << s << std::endl;
    logfile.close();
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
    handler[Expose]           = expose;
    handler[KeyPress]         = key_handler;
    handler[ButtonPress]      = buttonpress;
    handler[MotionNotify]     = motionnot;
    handler[ButtonRelease]    = buttonrelease;
    handler[PropertyNotify]   = propertynot;
}

void init_keys(Display* dpy) {
    for (unsigned int i = 0; i < sizeof(keys) / sizeof(*keys); i++) { XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym(keys[i].key)), keys[i].modifiers, DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync); }
    XGrabButton(dpy, 1, MOVE_KEY, DefaultRootWindow(dpy), True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
    XGrabButton(dpy, 3, MOVE_KEY, DefaultRootWindow(dpy), True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
}

void key_handler(Display* dpy, XEvent ev) {
    for (unsigned int i = 0; i < sizeof(keys) / sizeof(*keys); i++) {
        if ((keys[i].modifiers == ev.xkey.state) && (XKeysymToKeycode(dpy, XStringToKeysym(keys[i].key)) == ev.xkey.keycode)) { keys[i].function(dpy, ev, keys[i].arg); }
    }
}

void configurereq(Display* dpy, XEvent ev) {
    lll("configurereq");
    XWindowChanges wc;
    wc.x = ev.xconfigurerequest.x;
    wc.y = ev.xconfigurerequest.y;
    wc.width = ev.xconfigurerequest.width;
    wc.height = ev.xconfigurerequest.height;
    XConfigureWindow(dpy, ev.xconfigurerequest.window, ev.xconfigurerequest.value_mask, &wc);
    
}

void destroynot(Display* dpy, XEvent ev) {
    lll("destroynot");
    for (auto &i : winlist) {
        if (ev.xdestroywindow.window == i.wid) {
            i.destroy(dpy);
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
    if (active != nullptr) { active->unfocus(dpy); }
    XSelectInput(dpy, ev.xmaprequest.window, StructureNotifyMask | EnterWindowMask | FocusChangeMask | PropertyChangeMask);
    XMapWindow(dpy, ev.xmaprequest.window);
    //xgetgeometry trolling
    if (!found) { lll("NOT found window"); winlist.push_back(vswin(dpy, ev.xmaprequest.window, attr.x, attr.y, attr.width, attr.height)); lll("added new");}
    XSetInputFocus(dpy, ev.xmaprequest.window, RevertToParent, CurrentTime);
}

void enternot(Display* dpy, XEvent ev) {
    lll("enternot");
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

void expose(Display* dpy, XEvent ev) {
    for (auto &i : winlist) {
        if (ev.xexpose.window == i.t && (TITLEBAR_BUTTONS || TITLEBAR_NAME)) {
            for (auto &j : i.b) {
                j.decorate(dpy);
                j.text(dpy);
            }
            return;
        }
    }
    return;
}

void buttonpress(Display* dpy, XEvent ev) {
    lll("Button Press");
    if (active == nullptr) { return; }
    XRaiseWindow(dpy, active->t);
    // XRaiseWindow(dpy, active->wid);
    if ((ev.xbutton.window == active->t) && (ev.xbutton.subwindow == 0)) {
        start = ev.xbutton;
        start.subwindow = ev.xbutton.window;
        return;
    }
    if (ev.xbutton.subwindow == active->t) {
        start = ev.xbutton;
        start.subwindow = ev.xbutton.subwindow;
        return;
    }
    for (auto &i : active->b) {
        if (ev.xbutton.subwindow == i.bid) {
            if (i.bid == active->b.back().bid && TITLEBAR_NAME) { 
                start = ev.xbutton;
                start.subwindow = ev.xbutton.subwindow;
            } else if (TITLEBAR_BUTTONS) { 
                start.subwindow = None;
                i.function(dpy, ev, 0);
            }
            return;
        }
    }
    return;
}

void motionnot(Display* dpy, XEvent ev) {
    lll("Motion Notify");
    while(XCheckTypedEvent(dpy, MotionNotify, &ev));
    if (start.subwindow == None) { return; }
    int dx = ev.xbutton.x_root - start.x_root;
    int dy = ev.xbutton.y_root - start.y_root;
    active->move(dpy, start.button, dx, dy);
    return;
}

void buttonrelease(Display* dpy, XEvent ev) {
    lll("Button Release");
    XWindowAttributes attr;
    XGetWindowAttributes(dpy, active->t, &attr);
    active->x = attr.x;
    active->y = attr.y;
    active->w = attr.width;
    active->h = attr.height - TITLEBAR_HEIGHT;
    if (TITLEBAR_NAME) { 
        XGetWindowAttributes(dpy, active->b.back().bid, &attr);
        active->b.back().w = attr.width;
    }
    start.subwindow = None;
    // int dx = ev.xbutton.x_root - start.x_root;
    // int dy = ev.xbutton.y_root - start.y_root;
    // active->move(dpy, start.button, dx, dy);
    if (ev.xbutton.button == 2) {
        if (active != nullptr) { active->destroy(dpy); }
    }
}

void propertynot(Display* dpy, XEvent ev) {
    lll("Property Not");
    for (auto &i : winlist) {
        if (i.wid == ev.xproperty.window) {
            if (ev.xproperty.atom == XA_WM_NAME) {
                i.title(dpy);
            }
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
        if (handler[ev.type] != nullptr) { handler[ev.type](dpy, ev); }
    }
}
