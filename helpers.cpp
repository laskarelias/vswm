#include <X11/Xlib.h>
#include <unistd.h>
#include <iostream>

#include "helpers.h"

vswin::vswin(Window wid, int x, int y, unsigned int w, unsigned int h) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->wid = wid;
    this->a = true;
}

void vswin::focus() {
    a = true;
}

void vswin::unfocus() {
    a = false;
}
