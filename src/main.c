#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "x11.h"
#include "eventHandler.h"

#include "x11.c"
#include "eventHandler.c"

#define X11

#ifdef X11
#include <X11/Xlib.h>
__attribute__((noreturn)) static void quitWindow(EventHandler *eh) {
    XCloseDisplay(eh->x11Window->display);
    exit(0);
}
#else
static void quitWindow(EventHandler *eh) {
    (void)eh;
    assert(false && "NO protocol other than xorg supported");
    exit(1);
}
#endif

static void keyPress(EventHandler *eh, unsigned int keyCode) {
    switch (keyCode) {
        case 9: // ESC
            eh->quit(eh);
            break;
        default:
            break;
    }
}

int main(void) {
#ifndef X11
    assert(false && "Not Implemented");
    exit(1);
#endif
    X11Window window = createWindow(800, 800);
    EventHandler eh = eventHandler(&window, NULL);
    eh.quit = quitWindow;
    eh.keyPress = keyPress;
    eventHandlerStart(&eh);

    return 0;
}
