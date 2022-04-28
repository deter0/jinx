#pragma once

#include "x11.h"

// Don't set any events to NULL set them to `(void*)voidFn`
typedef struct EventHandler {
    X11Window *x11Window;
    void *userData;
    void (*keyPress)(struct EventHandler *eh, unsigned int keyCode);
    void (*render)(struct EventHandler *eh);
    void (*quit)(struct EventHandler *eh);
    void (*update)(struct EventHandler *eh, double dt);
    void (*click)(struct EventHandler *eh, float x, float y);
    void (*rightClick)(struct EventHandler *eh, float x, float y);
    void (*mouseMove)(struct EventHandler *eh, float x, float y);

    float mouseX;
    float mouseY;
} EventHandler;

void voidFn(void);
EventHandler eventHandler(X11Window *xWindow, void *userData);
void eventHandlerStart(EventHandler *eh);
