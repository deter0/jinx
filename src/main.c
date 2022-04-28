#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <math.h>

#include "jinx/jinx.h"
#include "jinx/jinx.c"
#include "src/jinx/core/events.h"
#include "src/jinx/core/helpers.h"
#include "src/jinx/core/jid.h"
#include "src/jinx/eventHandler.h"

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
    printf("no\n");
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


static int count = 0;
static char message[50];
ComponentTextRenderer *tren;

void onClick(JID *self, float x, float y, EventHandler *eh) {
    (void)x;
    (void)y;
    (void)self;
    count++;
    printf("Okay!\n");
    sprintf(message, "Clicked %d times!", count);
    tren->Text = message;
}

int main(void) {
#ifndef X11
    assert(false && "Not Implemented");
    exit(1);
#endif
    root = JIDRoot(800, 800); 
    ComponentVBoxLayout *layout = componentVBoxLayout(15, (Padding){
        .top = 12,
        .left = 12,
        .bottom = 12,
        .right = 12
    });
    layout->NoAutoSizeX = true;
    layout->NoAutoSizeY = true;
    JIDAddComp(root, (JIDComponent*)layout);
    JID *txt = JIDText(0, 0, "Click The Button");
    JIDSetParent(txt, root);

    JID *button = JIDTextButton(0, 0, "Button");
    ComponentEventHandler *ceh = (ComponentEventHandler*)getComponentHard(button, "ComponentEventHandler", NULL);
    ceh->onClick = onClick;

    JID *tracker = JIDText(0, 0, "");
    sprintf(message, "Clicked %d times!", count);
    tren = (ComponentTextRenderer*)getComponentSoft(tracker, "ComponentTextRenderer", NULL);
    tren->Text = message;
    tren->FontSize = 18;
    JIDSetFGColor(tracker, (RGBA){
        .r = 1,
        .g = 1,
        .b = 1,
        .a = 0.8
    });
    JIDSetParent(tracker, root);
    JIDSetParent(button, root);

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
    eh.update = update;
    eh.click = forwardClick;
    eventHandlerStart(&eh);

    return 0;
}
