#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>

#include "x11.h"
#include "eventHandler.h"

#include "x11.c"
#include "eventHandler.c"

#include "renderer/renderer.h"
#include "renderer/jid.h"

#include "renderer/jid.c"
#include "renderer/renderer.c"

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

static JID root;
static cairo_t *ctx;

static void render(EventHandler *eh) {
    (void)eh;
    renderRoot(&root, ctx);
    printf("Render!\n");
}

int main(void) {
#ifndef X11
    assert(false && "Not Implemented");
    exit(1);
#endif
    root = JIDRoot(800, 800);
    X11Window window = createWindow(800, 800);
    cairo_surface_t *surface = cairo_xlib_surface_create(
        window.display,
        window.window,
        DefaultVisual(window.display, DefaultScreen(window.display)),
        800, 800);
    ctx = cairo_create(surface);

    EventHandler eh = eventHandler(&window, NULL);
    eh.quit = quitWindow;
    eh.keyPress = keyPress;
    eh.render = render;
    eventHandlerStart(&eh);

    return 0;
}
