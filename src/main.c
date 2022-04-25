#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <math.h>

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


static JID *root;
static cairo_t *ctx;
static ComponentTransform *tf;

static void update(EventHandler *eh, double dt) {
    (void)dt;
    ComponentTransform *rt = (ComponentTransform*)getComponentHard(root, "ComponentTransform", "Program");
    rt->width = (float)eh->x11Window->attributes->width;
    rt->height = (float)eh->x11Window->attributes->height;
    eh->render(eh);
}

static void render(EventHandler *eh) {
    (void)eh;
    // tf->x = sin();
    renderRoot(root, ctx);
}

int main(void) {
#ifndef X11
    assert(false && "Not Implemented");
    exit(1);
#endif
    root = JIDRoot(800, 800);
    JID *rect = JIDRectangle(0, 0, 100, 100);
    ComponentColor *color =(ComponentColor*)getComponentHard(rect, "ComponentColor", "Program");
    color->background->r = 1.0f;
    color->background->g = 0.2f;
    color->background->b = 0.5f;
    tf =(ComponentTransform*)getComponentHard(rect, "ComponentTransform", "Program");
    JIDSetParent(rect, root);

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
