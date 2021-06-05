#pragma once

#ifndef HELPERS_H
#define HELPERS_H

typedef struct key_struct {
    int modifiers;
    char key[8];
    void (* function)(Display* dpy, XEvent ev, int arg);
    int arg; 
} key;

class vswin {
    public:
        Window wid;
        int x, y;
        unsigned int w, h;
        bool a;
        Window t;

        vswin(Display* dpy, Window wid, int x, int y, unsigned int w, unsigned int h);
        void destroy(Display* dpy);
        void focus(Display* dpy);
        void unfocus(Display* dpy);

        bool operator== (const vswin &b) { return (this->wid == b.wid); }
};

void configurereq(Display* dpy, XEvent ev);
void mapreq(Display* dpy, XEvent ev);
void destroynot(Display* dpy, XEvent ev);
void enternot(Display* dpy, XEvent ev);
void focusin(Display* dpy, XEvent ev);
void focusout(Display* dpy, XEvent ev);
void key_handler(Display* dpy, XEvent ev);

void close(Display* dpy, XEvent ev, int arg);
void info(Display* dpy, XEvent ev, int arg);

#endif