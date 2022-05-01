#include "jinx/jinx.h"
#include "src/jinx/core/jid.h"
#include <stdio.h>
#include <stdlib.h>

static void increaseSpacingFunc(JID *self, float x, float y, EventHandler *eh) {
    (void)x; (void)y;
    ((ComponentHBoxLayout*)getComponentHard(self->Parent, "ComponentHBoxLayout", NULL))->Spacing += 15;
    eh->render(eh);
}

static int count = 0;
void onClick(JID *self, float x, float y, EventHandler *eh) {
    (void)x;
    (void)y;
    (void)self;
    count++;
    char message[50];
    sprintf(message, "Clicked %d times!", count);
    ComponentTextRenderer *tren = (ComponentTextRenderer*)getComponentHard(self, "ComponentTextRenderer", NULL);
    tren->Text = message;
    eh->render(eh);
}

void app(JID *root) {
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

    JID *button = JIDTextButton(0, 0, "Click Me");
    ComponentEventHandler *ceh = (ComponentEventHandler*)getComponentHard(button, "ComponentEventHandler", NULL);
    ceh->onClick = onClick;

    JID *tracker = JIDText(0, 0, "Clicked 0 times");

    JIDSetFGColor(tracker, (RGBA){
        .r = 1,
        .g = 1,
        .b = 1,
        .a = 0.8
    });
    JIDSetParent(tracker, root);
    JIDSetParent(button, root);

    JIDSetParent(JIDText(0, 0, "Nested layout test"), root);
    JID *rect = JIDRectangle(0, 0, 100, 100);

    ComponentHBoxLayout *hlayout = componentHBoxLayout(15, (Padding){
        .top = 12,
        .left = 12,
        .bottom = 12,
        .right = 12
    });
    JIDAddComp(rect, (JIDComponent *)hlayout);
    // char buttonText[25];
    for (int i = 0; i < 5; i++) {
        JID *button = JIDTextButton(0, 0, "+Button");
        JIDSetParent(button, rect);
    }
    JID *increaseSpacing = JIDTextButton(0, 0, "+Spacing");
    ((ComponentEventHandler*)getComponentHard(increaseSpacing, "ComponentEventHandler", NULL))->onClick = increaseSpacingFunc;
    JIDSetParent(increaseSpacing, rect);
    JIDSetParent(rect, root);
}
