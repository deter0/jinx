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

// Put stuff like cairo_t inside userData
EventHandler eventHandler(X11Window *xWindow, void *userData) {
    return (EventHandler){
        .x11Window = xWindow,
        .userData = userData,
        .keyPress = defaultKeyPress,
        .quit = defaultQuit,
        .render = defaultRender,
        .update = defaultUpdate
    };
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
            double deltaUsec = (curr.tv_usec - last.tv_usec) / (double)1000;
            gettimeofday(&last, NULL);
            eh->update(eh, deltaUsec);
            XNextEvent(window.display, &event);
            // if (XPending(window.display)) {

                switch (event.type) {
                    case Expose:
                        XGetWindowAttributes(window.display,
                            window.window,
                            window.attributes);
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
            // } else {
            //     usleep(16000);
            // }
        }
        eh->quit(eh);
    } else {
        assert(false && "Not implemented/ unreachable");
    }
}
