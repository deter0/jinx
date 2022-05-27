#include "jinx/jinx.h"
#include "src/jinx/core/helpers.h"
#include "src/jinx/core/jid.h"
#include "src/jinx/eventHandler.h"
#include "src/jinx/jinxst/jinxst.h"
#include <stdio.h>
#include <stdlib.h>

static void increaseSpacingFunc(JID *self, float x, float y, EventHandler *eh) {
    (void)x; (void)y;
    ((ComponentHBoxLayout*)getComponentHard(self->Parent, "ComponentHBoxLayout", NULL))->Spacing += 15;
    Damage(self->Parent);
    eh->render(eh, false);
}

static int count = 0;
void onClick(JID *self, float x, float y, EventHandler *eh) {
    (void)x;
    (void)y;
    count++;
    char *message = malloc(50);
    sprintf(message, "Clicked %d times!", count);
    JIDSetText(self, message);
    Damage(self->Parent);
    eh->render(eh, true);
}

void exitApp() {
    printf("Hello!\n");
}

JID *rootG;
void onMouseUp(float _x, float _y, EventHandler *eh) {
    printf("UP!\n");
    eh->quit(eh);
}

JID *fileContainer(const char *bufferName) {
    JID *filesContainer = JIDRectangle(0, 70.0f, 600, 400);
    ComponentRelativeTransform *rtf = componentRelativeTransform();
    rtf->percentHeight = 0.80f;
    rtf->percentWidth = 0.80f;
    rtf->y = 0.0f;
    rtf->x = 0.0f;
    ComponentRectangleRenderer* rtr = (ComponentRectangleRenderer*)getComponentHard(filesContainer, "ComponentRectangleRenderer", NULL);
    rtr->BorderRadius = 4.0;
    JIDAddComp(filesContainer, (JIDComponent*)rtf);
    JIDSetBGColor(filesContainer, *jinxstGetColor("global", "header_bg"));

    ComponentVBoxLayout *vbx = componentVBoxLayout(12, (Padding) {0});
    JIDAddComp(filesContainer, (JIDComponent*)vbx);

    JID *bufferHeader = JIDRectangle(0, 0, 400.0f, 28.0f);
        rtr = (ComponentRectangleRenderer*)getComponentHard(bufferHeader, "ComponentRectangleRenderer", NULL);
        rtr->BorderRadius = 4.0f;
        rtf = componentRelativeTransform();
        rtf->x = 0.0f;
        rtf->y = 0.0f;
        rtf->percentWidth = 0.7f;
        rtf->height = 36.0f;
        JIDSetBGColor(bufferHeader, (RGBA){
            .r = 0.4f,
            .b = 0.4f,
            .g = 0.4f,
            .a = 1.0f,
        });
        JIDAddComp(bufferHeader, (JIDComponent*)rtf);

        ComponentHBoxLayout *hbx = componentHBoxLayout(12.0f, (Padding){
            6.0f, 6.0f, 6.0f, 6.0f
        });
        JIDAddComp(bufferHeader, (JIDComponent*)hbx);

        JID *text = JIDText(0, 0, bufferName);
        ComponentTextRenderer *tren = (ComponentTextRenderer *)getComponentHard(text, "ComponentTextRenderer", NULL);
        tren->FontSize = 20.0f;
        tren->Font = "JetBrainsMono Nerd Font";
        JIDSetParent(text, bufferHeader);
    JIDSetParent(bufferHeader, filesContainer);

    return filesContainer;
}

void app(JID *root) {
    rootG = root;
    ComponentVBoxLayout *layout = componentVBoxLayout(15, (Padding){
        .top = 12,
        .left = 12,
        .bottom = 12,
        .right = 12
    });
    layout->NoAutoSizeX = true;
    layout->NoAutoSizeY = true;
    
    JID *header = JIDRectangle(0, 0, 0, 0);
    ComponentRelativeTransform *rtf = componentRelativeTransform();
    rtf->height = 57.5f;
    rtf->percentWidth = 1.0f;
    JIDAddComp(header, (JIDComponent*)rtf);
    JIDSetBGColor(header, *jinxstGetColor("global", "header_bg"));

    ComponentHBoxLayout *hbl = componentHBoxLayout(10, (Padding){
        .top = 10,
        .bottom = 10,
        .left = 10,
        .right = 10
    });
    JIDAddComp(header, (JIDComponent*)hbl);

    JID *openFileButton = JIDTextButton(0, 0, "Open File");
    JIDSetParent(openFileButton, header);
    JIDSetParent(header, root);

    JID *filesContainer = JIDRectangle(10.0f, 67.5f, 100, 0);
    ComponentHBoxLayout *hbx = componentHBoxLayout(10, (Padding){0});
    hbx->NoAutoSizeY = true;
    rtf = componentRelativeTransform();
    rtf->height = 800.0f;
    rtf->percentWidth = 1.0f;
    rtf->x = 10.0f;
    rtf->y = 67.5f;
    JIDAddComp(filesContainer, (JIDComponent*)rtf);
    JIDAddComp(filesContainer, (JIDComponent*)hbx);
    JIDSetBGColor(filesContainer, (RGBA){0});

    JIDSetParent(fileContainer("Scratch Buffer :^)"), filesContainer);
    JIDSetParent(fileContainer("Scratch Buffer 2 :^)"), filesContainer);
    JIDSetParent(filesContainer, root);
}
