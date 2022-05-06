#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <math.h>

#include "app.c"

#include "jinx/jinx.h"
#include "jinx/jinx.c"
#include "src/jinx/core/events.h"
#include "src/jinx/core/helpers.h"
#include "src/jinx/core/jid.h"
#include "src/jinx/core/renderer.h"
#include "src/jinx/eventHandler.h"
#include "src/jinx/slurp.h"

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

static void render(EventHandler *eh) {
    (void)eh;
    ComponentTransform *tf = (ComponentTransform*)getComponentHard(root, "ComponentTransform", NULL);
    tf->width = eh->x11Window->attributes->width;
    tf->height = eh->x11Window->attributes->height;
    renderRoot(root, ctx);
}

static void mouseMove(EventHandler *eh, float x, float y) {
    JIDOnMouseMove(root, x, y, eh);
}
static void forwardClick(EventHandler *eh, float x, float y) {
    JIDOnMouseClick(root, x, y, eh);
}

int main(void) {
#ifndef X11
    assert(false && "Not Implemented");
    exit(1);
#endif
#if 1
    char *defaultJinxstSrc = jinxSlurpFile("../defaults/default.jinxst");
    assert(defaultJinxstSrc != NULL);
    jinxst(defaultJinxstSrc);
    exit(0);
#endif
    root = JIDRoot(800, 800); 

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
    eh.mouseMove = mouseMove;
    eh.click = forwardClick;

    app(root);
    renderRoot(root, ctx);
    updateLayouts(root);

    eventHandlerStart(&eh);

    return 0;
}
