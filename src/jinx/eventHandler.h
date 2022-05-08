#pragma once

#include "x11.h"

#define MAX_CONNECTIONS 15

// Don't set any events to NULL set them to `(void*)voidFn`
typedef struct EventHandler {
    X11Window *x11Window;
    void *userData;
    void (*keyPress)(struct EventHandler *eh, unsigned int keyCode);
    void (*render)(struct EventHandler *eh);
    void (*quit)(struct EventHandler *eh);
    void (*update)(struct EventHandler *eh, double dt);
    void (*click)(struct EventHandler *eh, float x, float y);
    void (*clickUp)(struct EventHandler *eh, float x, float y);
    void (*rightClick)(struct EventHandler *eh, float x, float y);
    void (*rightClickUp)(struct EventHandler *eh, float x, float y);
    void (*mouseMove)(struct EventHandler *eh, float x, float y);

    void (*mouseMoveConnections[MAX_CONNECTIONS])(struct EventHandler *eh, float x, float y);
    size_t mouseMoveConnectionsCount;

    void (*mouseUpConnections[MAX_CONNECTIONS])(struct EventHandler *eh, float x, float y);
    size_t mouseUpConnectionsCount;

    float mouseX;
    float mouseY;
} EventHandler;

void voidFn(void);
EventHandler eventHandler(X11Window *xWindow, void *userData);
void eventHandlerStart(EventHandler *eh);

size_t eventHandlerConnectMouseMove(EventHandler *eh, void (*callback)(struct EventHandler *eh, float x, float y));
void eventHandlerDisconnectMouseMove(EventHandler *eh, size_t index);

size_t eventHandlerConnectMouseUp(EventHandler *eh, void (*callback)(struct EventHandler *eh, float x, float y));
void eventHandlerDisconnectMouseUp(EventHandler *eh, size_t index);
