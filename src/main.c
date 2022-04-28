#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <math.h>

#include "jinx/jinx.h"
#include "jinx/jinx.c"
#include "src/jinx/core/helpers.h"
#include "src/jinx/core/jid.h"

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
    ComponentTransform *tf = (ComponentTransform*)getComponentHard(root, "ComponentTransform", NULL);
    tf->width = eh->x11Window->attributes->width;
    tf->height = eh->x11Window->attributes->height;
    renderRoot(root, ctx);
}

int main(void) {
#ifndef X11
    assert(false && "Not Implemented");
    exit(1);
#endif
    root = JIDRoot(800, 800); 
    ComponentVBoxLayout *layout = componentVBoxLayout(90, (Padding){
        .top = 12,
        .left = 12,
        .bottom = 12,
        .right = 12
    });
    JIDAddComp(root, (JIDComponent*)layout);
    JID *txt = JIDText(0, 0, "Click The Button");
    JIDSetBGColor(txt, (RGBA){
        .r = 1.0,
        .g = 0.0,
        .b = 0.0,
        .a = 0.5
    });
    JIDSetParent(txt, root);
    JID *txt2 = JIDText(0, 0, "Slide Me");
    JIDSetBGColor(txt2, (RGBA){
        .r = 1.0,
        .g = 0.0,
        .b = 0.0,
        .a = 0.5
    });
    JIDSetParent(txt2, root);

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
