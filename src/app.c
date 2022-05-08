#include "jinx/jinx.h"
#include "src/jinx/core/helpers.h"
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
    char *message = malloc(50);
    sprintf(message, "Clicked %d times!", count);
    printf("%s\n", message);
    ComponentTextRenderer *tren = (ComponentTextRenderer*)getComponentHard(self, "ComponentTextRenderer", NULL);
    if (count > 1)
        free(tren->Text);
    tren->Text = message;
    // tren->Text = message;
    eh->render(eh);
}

void onMouseUp() {
    printf("UP!\n");
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

    JIDSetParent(button, root);

    JIDSetParent(JIDText(0, 0, "Nested layout test"), root);
    JID *rect = JIDRectangle(0, 0, 100, 100);
    ((ComponentRectangleRenderer*)getComponentHard(rect, "ComponentRectangleRenderer", NULL))
        ->BorderRadius = 6;
    JIDSetBGColor(rect, (RGBA){
        .r = 46 / 255.0,
        .g = 46 / 255.0,
        .b = 46 / 255.0,
        .a = 1.0
    });

    ComponentHBoxLayout *hlayout = componentHBoxLayout(15, (Padding){
        .top = 12,
        .left = 12,
        .bottom = 12,
        .right = 12
    });
    JIDAddComp(rect, (JIDComponent *)hlayout);
    // char buttonText[25];
    for (int i = 0; i < 5; i++) {
        // JID *button = JIDTextButton(0, 0, "+Button");
        JID *rect2 = JIDRectangle(0, 0, 0, 0);
        ComponentVBoxLayout *vlayout = componentVBoxLayout(5, (Padding){
            .top = 5,
            .left = 5,
            .right = 5,
            .bottom = 5
        });
        // JIDSetParent(button, rect);
        JIDAddComp(rect2, (JIDComponent*)vlayout);
        for (int i = 0; i < 5; i++) {
            JID *button2 = JIDTextButton(0, 0, "Button LLOL!");
            JIDSetParent(button2, rect2);
        }
        JIDSetParent(rect2, rect);
    }
    JID *increaseSpacing = JIDTextButton(0, 0, "+Spacing");
    ((ComponentEventHandler*)getComponentHard(increaseSpacing, "ComponentEventHandler", NULL))->onClick = increaseSpacingFunc;
    // ((ComponentEventHandler*)getComponentHard(increaseSpacing, "ComponentEventHandler", NULL))->onClickUp = onMouseUp;
    JIDSetParent(increaseSpacing, rect);
    JIDSetParent(rect, root);
}
