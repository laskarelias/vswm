#pragma once

#ifndef HELPERS_H
#define HELPERS_H

typedef struct key_struct {
    unsigned int modifiers;
    char key[8];
    void (* function)(Display* dpy, XEvent ev, int arg);
    int arg; 
} key;

typedef struct mouse_struct {
    unsigned int modifiers;
    int btn;
    void (* function)(Display* dpy, XEvent ev, int arg);
    int arg; 
} mouse;

class button;

class vswin {
    public:
        Window wid;
        int x, y;
        unsigned int w, h;
        bool a;
        Window t;
        std::list<button> b;

        vswin(Display* dpy, Window wid, int x, int y, unsigned int w, unsigned int h);
        void destroy(Display* dpy);
        void focus(Display* dpy);
        void unfocus(Display* dpy);
        void move(Display* dpy, int btn, int x, int y);

        bool operator== (const vswin &b) { return (this->wid == b.wid); }
};

class button {
    public:
        Window bid;
        int x;
        unsigned int w;
        vswin* p;
        void (* function) (Display* dpy, XEvent ev, int arg);

        button(Display* dpy, vswin* p, void (* function)(Display* dpy, XEvent ev, int arg));

        bool operator== (const button &b) { return (this->bid == b.bid); }
};

void configurereq(Display* dpy, XEvent ev);
void mapreq(Display* dpy, XEvent ev);
void destroynot(Display* dpy, XEvent ev);
void enternot(Display* dpy, XEvent ev);
void focusin(Display* dpy, XEvent ev);
void focusout(Display* dpy, XEvent ev);
void key_handler(Display* dpy, XEvent ev);
void buttonpress(Display* dpy, XEvent ev);
void motionnot(Display* dpy, XEvent ev);
void buttonrelease(Display* dpy, XEvent ev);

void close(Display* dpy, XEvent ev, int arg);
void info(Display* dpy, XEvent ev, int arg);

#endif