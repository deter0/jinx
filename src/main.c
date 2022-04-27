#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <math.h>

#include "jinx/jinx.h"
#include "jinx/jinx.c"
#include "src/jinx/core/helpers.h"

#define X11

__attribute__((noreturn)) static void quitWindow(EventHandler *eh) {
    XCloseDisplay(eh->x11Window->display);
    exit(0);
}

static void keyPress(EventHandler *eh, unsigned int keyCode) {
    switch (keyCode) {
        case 9: // ESC
            eh->quit(eh);
            break;
        default:
            break;
    }
}

static JID *root;
static cairo_t *ctx;

static void update(EventHandler *eh, double dt) {
    (void)dt;
    (void)eh;
}

static void render(EventHandler *eh) {
    (void)eh;
    renderRoot(root, ctx);
}

int main(void) {
#ifndef X11
    assert(false && "Not Implemented");
    exit(1);
#endif
    root = JIDRoot(800, 800); 
    JID *txt = JIDText(0, 0, "Click the button:");
    assert(JIDSetText(txt, "Click the button please!") != 1);
    assert(JIDSetFGColor(txt, (RGBA){
        .r = 255 / 255.0,
        .g = 255 / 255.0,
        .b = 25 / 255.0,
        .a = 1
    }) != 1);
    JIDSetParent(txt, root);

    X11Window window = createWindow(800, 800);
    cairo_surface_t *surface = cairo_xlib_surface_create(
        window.display,
        window.window,
        DefaultVisual(window.display, DefaultScreen(window.display)),
        1920, 1080);
    ctx = cairo_create(surface);

    EventHandler eh = eventHandler(&window, NULL);
    eh.quit = quitWindow;
    eh.keyPress = keyPress;
    eh.render = render;
    eh.update = update;
    eventHandlerStart(&eh);

    return 0;
}
