#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "x11.h"
#include "eventHandler.h"

static void defaultQuit(EventHandler *eh) { (void)eh; }
static void defaultKeyPress(EventHandler *eh, unsigned int keyCode) { (void)eh; (void)keyCode; }
static void defaultRender(EventHandler *eh) { (void)eh; }

// Put stuff like cairo_t inside userData
EventHandler eventHandler(X11Window *xWindow, void *userData) {
    return (EventHandler){
        .x11Window = xWindow,
        .userData = userData,
        .keyPress = defaultKeyPress,
        .quit = defaultQuit,
        .render = defaultRender,
    };
}

void eventHandlerStart(EventHandler *eh) {
    if (eh->x11Window != NULL) {
        X11Window window = *eh->x11Window;
        Atom deleteWindow = XInternAtom(window.display, "WM_DELETE_WINDOW", false);
        XSetWMProtocols(window.display, window.window, &deleteWindow, 1);

        int running = 1;
        XEvent event;
        while (running) {
            XNextEvent(window.display, &event);

            switch (event.type) {
                case Expose:
                    eh->render(eh);
                    break;
                case ButtonPress:
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
