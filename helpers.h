#ifndef HELPERS_H
#define HELPERS_H

class vswin {
    public:
        Window wid;
        int x, y;
        unsigned int w, h;
        bool a;
        Window t;

        vswin(Display* dpy, Window wid, int x, int y, unsigned int w, unsigned int h);
        void focus();
        void unfocus();

};

void configurereq(Display* dpy, XEvent ev);
void mapreq(Display* dpy, XEvent ev);
void enternot(Display* dpy, XEvent ev);
void focusin(Display* dpy, XEvent ev);
void focusout(Display* dpy, XEvent ev);


#endif