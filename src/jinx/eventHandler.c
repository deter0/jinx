#include <X11/X.h>
#include <X11/Xlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include "src/jinx/x11.h"
#include "src/jinx/eventHandler.h"

static void defaultQuit(EventHandler *eh) { (void)eh; }
static void defaultKeyPress(EventHandler *eh, unsigned int keyCode) { (void)eh; (void)keyCode; }
static void defaultRender(EventHandler *eh) { (void)eh; }
static void defaultUpdate(EventHandler *eh, double dt) { (void)eh; (void)dt; }
static void defaultClickHandler(EventHandler *eh, float x, float y) { (void)eh; (void)x; (void)y; }

// Put stuff like cairo_t inside userData
EventHandler eventHandler(X11Window *xWindow, void *userData) {
    return (EventHandler){
        .x11Window = xWindow,
        .userData = userData,
        .keyPress = defaultKeyPress,
        .quit = defaultQuit,
        .render = defaultRender,
        .update = defaultUpdate,
        .click = defaultClickHandler,
        .clickUp = defaultClickHandler,
        .rightClick = defaultClickHandler,
        .rightClickUp = defaultClickHandler,
        .mouseMove = defaultClickHandler,

        .mouseMoveConnections = {0},
        .mouseMoveConnectionsCount = 0,

        .mouseUpConnections = {0},
        .mouseUpConnectionsCount = 0
    };
}

/**
    * adds a connection to mouse move
    *
    * @return - Returns index so you can disconnect this event later
*/
size_t eventHandlerConnectMouseMove(EventHandler *eh, void (*callback)(struct EventHandler *eh, float x, float y)) {
    assert(callback != NULL);
    assert(eh->mouseMoveConnectionsCount < MAX_CONNECTIONS); // TODO: Error info
    eh->mouseMoveConnections[eh->mouseMoveConnectionsCount] = callback;
    eh->mouseMoveConnectionsCount++;    
    return eh->mouseMoveConnectionsCount - 1;
}

void eventHandlerDisconnectMouseMove(EventHandler *eh, size_t index) {
    assert(eh->mouseMoveConnectionsCount >= index);
    for (size_t i = index; i < eh->mouseMoveConnectionsCount - 1; eh++) {
        eh->mouseMoveConnections[i] = eh->mouseMoveConnections[i + 1];
    }
    eh->mouseMoveConnectionsCount--;
}

/**
    * adds a connection to mouse move
    *
    * @return - Returns index so you can disconnect this event later
*/
size_t eventHandlerConnectMouseUp(EventHandler *eh, void (*callback)(struct EventHandler *eh, float x, float y)) {
    assert(callback != NULL);
    assert(eh->mouseUpConnectionsCount < MAX_CONNECTIONS); // TODO: Error info
    eh->mouseUpConnections[eh->mouseUpConnectionsCount] = callback;
    eh->mouseUpConnectionsCount++;    
    return eh->mouseUpConnectionsCount - 1;
}

void eventHandlerDisconnectMouseUp(EventHandler *eh, size_t index) {
    assert(eh->mouseUpConnectionsCount >= index);
    for (size_t i = index; i < eh->mouseUpConnectionsCount - 1; eh++) {
        eh->mouseUpConnections[i] = eh->mouseUpConnections[i + 1];
    }
    eh->mouseUpConnectionsCount--;
}

void eventHandlerStart(EventHandler *eh) {
    if (eh->x11Window != NULL) {
        X11Window window = *eh->x11Window;
        Atom deleteWindow = XInternAtom(window.display, "WM_DELETE_WINDOW", false);
        XSetWMProtocols(window.display, window.window, &deleteWindow, 1);

        int running = 1;
        XEvent event;
        struct timeval curr, last;
        gettimeofday(&last, NULL);
        while (running) {
            gettimeofday(&curr, NULL);
            // double deltaUsec = (curr.tv_usec - last.tv_usec) / (double)1000;
            gettimeofday(&last, NULL);
            // eh->update(eh, deltaUsec);
            XNextEvent(window.display, &event);
            // if (eh->update == NULL) {
            // } else {
            //     if (XPending(window.display)) {
            //         XNextEvent(window.display, &event);
            //     } else {
            //         usleep(1600);
            //     }
            // }
            switch (event.type) {
                case Expose:
                    if (event.xexpose.count == 0) {
                        XGetWindowAttributes(window.display,
                            window.window,
                            window.attributes);
                        eh->render(eh);
                    }
                    break;
                case MotionNotify:
                    eh->mouseX = (float)event.xmotion.x;
                    eh->mouseY = (float)event.xmotion.y;
                    eh->mouseMove(eh, eh->mouseX, eh->mouseY);
                    for (size_t i = 0; i < eh->mouseMoveConnectionsCount; i++) {
                        if (eh->mouseMoveConnections[i] != NULL)
                            eh->mouseMoveConnections[i](eh, event.xmotion.x, event.xmotion.y);
                    }
                    break;
                case ButtonRelease:
                    switch (event.xbutton.button) {
                        case 1:
                            eh->clickUp(eh, (float)event.xbutton.x, (float)event.xbutton.y);
                            break;
                        case 3:
                            eh->rightClickUp(eh, (float)event.xbutton.x, (float)event.xbutton.y);
                            break;
                        default:
                            fprintf(stderr, "Unhandled click (up): %d\n", event.xbutton.button);
                    }
                    for (size_t i = 0; i < eh->mouseUpConnectionsCount; i++) {
                        if (eh->mouseUpConnections[i] != NULL)
                            eh->mouseUpConnections[i](eh, event.xbutton.x, event.xbutton.y);
                    }
                    break;
                case ButtonPressMask:
                    // printf("%d\n", event.xbutton.state);
                    switch (event.xbutton.button) {
                        case 1:
                            eh->click(eh, (float)event.xbutton.x, (float)event.xbutton.y);
                            break;
                        case 3:
                            eh->rightClick(eh, (float)event.xbutton.x, (float)event.xbutton.y);
                            break;
                        default:
                            fprintf(stderr, "Unhandled click: %d\n", event.xbutton.button);
                    }
                    break;
                case KeyPress:
                    eh->keyPress(eh, event.xkey.keycode);
                    break;
                case ClientMessage:
                    if ((Atom)(event.xclient.data.l[0]) == deleteWindow) {
                        running = false;
                    }
                    break;
                default:
                    break;
            }
        }
        eh->quit(eh);
    } else {
        assert(false && "Not implemented/ unreachable");
    }
}
