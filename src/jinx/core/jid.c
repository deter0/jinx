// JID: Jinx Widget

#include "src/jinx/core/jid.h"
#include "src/jinx/core/renderer.h"
#include "src/jinx/core/renderFunctions.h"
#include "src/jinx/core/layouts.h"
#include "src/jinx/core/helpers.h"
#include "src/jinx/eventHandler.h"

#include "../jinxst/jinxst.h"
#include "src/jinx/jinxst/jinxstTable.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

static void *scalloc(size_t s) {
    void *res = calloc(1, s);
    if (res == NULL) {
        fprintf(stderr, "Error allocating memory: %ld, Exiting.\nb", s);
        exit(1);
    }
    return res;
}

JIDComponent *getComponentHard(JID *jid, const char *target, const char *from) {
    int index = JIDFindComp(jid, target);
    if (index == -1) {
        fprintf(stderr, "Component: `%s` requires component: `%s`\n", from,
            target);
        exit(1);
    }
    return jid->Components[index];
}
JIDComponent *getComponentSoft(JID *jid, const char *target, const char *from) {
    int index = JIDFindComp(jid, target);
    if (index == -1) {
        if (from != NULL)
            fprintf(stderr, "Component `%s` recommends component: `%s`\n", from,
                target);
        return NULL;
    }
    return jid->Components[index];
}

// Components

ComponentTransform *componentTransform(float x, float y, float w, float h) {
    ComponentTransform *transform = (ComponentTransform*)scalloc(sizeof(ComponentTransform));
    transform->ComponentName = "ComponentTransform";
    transform->x = x;
    transform->y = y;
    transform->width = w;
    transform->height = h;
    transform->isLayout = false;
    return transform;
}

ComponentRelativeTransform *componentRelativeTransform(void) {
    ComponentRelativeTransform *rt = (ComponentRelativeTransform*)
                                        scalloc(sizeof(ComponentRelativeTransform));
    rt->ComponentName = "ComponentRelativeTransform";
    rt->isLayout = false;
    rt->isRenderable = false;
    rt->percentHeight = 0.0f;
    rt->percentWidth = 0.0f;
    rt->width = 0.0f;
    rt->height = 0.0f;
    rt->x = 0.0f;
    rt->y = 0.0f;
    rt->percentX = 0.0f;
    rt->percentWidth = 0.0f;
    return rt;
}

ComponentTextRenderer *componentTextRenderer(void) {
    ComponentTextRenderer *rndr = (ComponentTextRenderer*)scalloc(sizeof(ComponentTextRenderer));
    rndr->ComponentName = "ComponentTextRenderer";
    rndr->FontSize = 24;
    rndr->Text = "";
    rndr->render = renderTextRenderer;
    rndr->isRenderable = true;
    rndr->isLayout = false;
    rndr->isHovering = false;
    rndr->Padding = (Padding){
        .top = 0,
        .bottom = 0,
        .left = 0,
        .right = 0
    };
    return rndr;
}

////

bool JIDTypeEq(JID *jid, const char *expect) {
    return !strcmp(jid->JIDType, expect);
}

JID *JIDRoot(float width, float height) {
    JID *root = scalloc(sizeof(JID));

    root->JIDType ="Root";
    root->Children = NULL;
    root->ChildrenCount = 0;
    root->ChildrenAlloc = 0;
    root->ComponentCount = 0;

    ComponentTransform *transform = componentTransform(0, 0, width, height);
    assert(JIDAddComp(root, (JIDComponent*)transform) != 1);
    ComponentColor *color = componentColorBG(1.0, 1.0, 1.0, 0.0);
    assert(JIDAddComp(root, (JIDComponent*)color) != 1);
    ComponentRectangleRenderer *rectangleRenderer = componentRectangleRenderer();
    assert(JIDAddComp(root, (JIDComponent*)rectangleRenderer) != 1);
    

    if (globalStyles != NULL) {
        Variable *var = ht_search(globalStyles->Properties, "background_color");
        if (var != NULL) {
            assert(strcmp(var->type, "color") == 0);
            assert(JIDSetBGColor(root, var->colorValue) != 1);
        } else
            goto applyDefaults;
    } else {
        applyDefaults:
            fprintf(stderr, "globalStyles is null!\n");
            assert(JIDSetBGColor(root, (RGBA){
                .r = 20 / 255.0,
                .g = 22 / 255.0,
                .b = 21 / 255.0,
                .a = 1.0
            }) != 1);
    }

    return root;
}

JID *JIDRectangle(float x, float y, float width, float height) {
    JID *rect = scalloc(sizeof(JID));

    rect->JIDType ="Rectangle";
    rect->Children = NULL;
    rect->ChildrenCount = 0;
    rect->ChildrenAlloc = 0;
    rect->ComponentCount = 0;

    ComponentTransform *transform = componentTransform(x, y, width, height);
    assert(JIDAddComp(rect, (JIDComponent*)transform) != 1);
    ComponentColor *color = componentColorBG(1.0, 1.0, 1.0, 1.0);
    assert(JIDAddComp(rect, (JIDComponent*)color) != 1);
    ComponentRectangleRenderer *rectangleRenderer = componentRectangleRenderer();
    assert(JIDAddComp(rect, (JIDComponent*)rectangleRenderer) != 1);

    return rect;
}

JID *JIDText(float x, float y, char *text) {
    JID *rect = scalloc(sizeof(JID));

    rect->JIDType ="Text";
    rect->Children = NULL;
    rect->ChildrenCount = 0;
    rect->ChildrenAlloc = 0;
    rect->ComponentCount = 0;

    const int fontSize = 24;
    ComponentTransform *transform = componentTransform(x, y, strlen(text) * fontSize, fontSize);
    assert(JIDAddComp(rect, (JIDComponent*)transform) != 1);
    ComponentColor *color = componentColorFG(1.0, 1.0, 1.0, 1.0);
    assert(JIDAddComp(rect, (JIDComponent*)color) != 1);
    ComponentTextRenderer *textRenderer = componentTextRenderer();
    textRenderer->Text = text;
    textRenderer->FontSize = fontSize;
    assert(JIDAddComp(rect, (JIDComponent*)textRenderer) != 1);
    assert(JIDSetBGColor(rect, (RGBA){
        .r = 1.0,
        .g = 1.0,
        .b = 1.0,
        .a = 0.0
    }) != 1); // TODO: Unhardcode

    if (globalStyles != NULL) {
        Variable *var = ht_search(globalStyles->Properties, "text_color");
        if (var != NULL) {
            assert(strcmp(var->type, "color") == 0);
            assert(JIDSetFGColor(rect, var->colorValue) != 1);
        } else
            goto applyDefaults;
    } else {
        applyDefaults:
            fprintf(stderr, "globalStyles is null!\n");
            assert(JIDSetBGColor(rect, (RGBA){
                .r = 1.0,
                .g = 1.0,
                .b = 1.0,
                .a = 1.0
            }) != 1);
    }

    return rect;
}

ComponentEventHandler *componentEventHandler(void) {
    ComponentEventHandler *ceh = scalloc(sizeof(ComponentEventHandler));
    ceh->ComponentName = "ComponentEventHandler";
    ceh->isLayout = false;
    ceh->isRenderable = false;
    ceh->onClick = NULL;
    ceh->onMouseEnter = NULL;
    ceh->onMouseLeave = NULL;
    return ceh;
}

void defaultMouseEnter(JID *self, float x, float y, EventHandler *eh) {
    (void)x;
    (void)y;
    JIDSetBGColor(self, (RGBA){
        .r = 155 / 255.0,
        .g = 122 / 255.0,
        .b = 240 / 255.0,
        .a = 1.0
    });
    eh->render(eh);
}
void defaultMouseLeave(JID *self, float x, float y, EventHandler *eh) {
    (void)x;
    (void)y;
    JIDSetBGColor(self, (RGBA){
        .r = 120 / 255.0,
        .g = 75 / 255.0,
        .b = 236 / 255.0,
        .a = 1.0
    });
    eh->render(eh);
}

JID *JIDTextButton(float x, float y, char *text) {
    JID *rect = scalloc(sizeof(JID));

    rect->JIDType = "TextButton";
    rect->Children = NULL;
    rect->ChildrenCount = 0;
    rect->ChildrenAlloc = 0;
    rect->ComponentCount = 0;

    const int fontSize = 20;
    ComponentTransform *transform = componentTransform(x, y, strlen(text) * fontSize, fontSize);
    assert(JIDAddComp(rect, (JIDComponent*)transform) != 1);
    ComponentColor *color = componentColorFG(1.0, 1.0, 1.0, 1.0);
    assert(JIDAddComp(rect, (JIDComponent*)color) != 1);
    ComponentTextRenderer *textRenderer = componentTextRenderer();
    textRenderer->Text = text;
    textRenderer->FontSize = fontSize;
    textRenderer->Padding = (Padding){
        .top = 12,
        .bottom = 12,
        .left = 12,
        .right = 12
    };

    ComponentRectangleRenderer *rectangle = componentRectangleRenderer();
    rectangle->BorderRadius = 6;
    JIDSetBGColor(rect, (RGBA){
        .r = 120 / 255.0,
        .g = 75 / 255.0,
        .b = 236 / 255.0,
        .a = 1.0
    });
    ComponentEventHandler *ceh = componentEventHandler();
    ceh->onMouseEnter = defaultMouseEnter;
    ceh->onMouseLeave = defaultMouseLeave;

    assert(JIDAddComp(rect, (JIDComponent*)rectangle) != 1);
    assert(JIDAddComp(rect, (JIDComponent*)textRenderer) != 1);
    assert(JIDAddComp(rect, (JIDComponent*)ceh) != 1);

    return rect;
}

JID *currentDrag;
static float startX;
static float initalX;
static bool dragging = false;
static size_t sliderMouseMoveIndex = 0;
static size_t sliderMouseUpIndex = 0;

#define DRAGGER_SIZE 18.0f

static void sliderMouseMove(EventHandler *eh, float x, float y) {
    (void)y;
    if (dragging) {
        float delta = x - (float)startX;
        ComponentRelativeTransform *rt = (ComponentRelativeTransform*)getComponentHard(currentDrag, "ComponentRelativeTransform", NULL);
        assert(currentDrag->Parent != NULL);
        ComponentTransform *parentTransform = (ComponentTransform*)getComponentHard(currentDrag->Parent, "ComponentTransform", NULL);

        float realWidth = parentTransform->width - DRAGGER_SIZE;
        rt->x = initalX + delta;
        rt->x = (rt->x < 0 ? 0 : (rt->x > realWidth ? realWidth : rt->x));
        eh->render(eh);
    } else {
        printf("Done!!\n");
        eventHandlerDisconnectMouseMove(eh, sliderMouseMoveIndex);
    }
}

static void sliderMouseUp(EventHandler *eh, float x, float y) {
    (void)eh;
    (void)x;
    (void)y;
    ComponentRelativeTransform *rt = (ComponentRelativeTransform*)getComponentHard(currentDrag, "ComponentRelativeTransform", NULL);
    initalX = rt->x;
    dragging = false;
    startX = 0;
}

static void sliderMouseDown(JID *self, float x, float y, EventHandler *eh) {
    (void)y;
    printf("Clicked!\n");
    startX = x;
    dragging = true;
    currentDrag = self;
    sliderMouseMoveIndex = eventHandlerConnectMouseMove(eh, sliderMouseMove);
    sliderMouseUpIndex = eventHandlerConnectMouseUp(eh, sliderMouseUp);
}

JID *JIDSlider(float x, float y, float w, float h) {
    JID *sliderRoot = JIDRectangle(x, y, w, h);
    ComponentRectangleRenderer *rootRect =
        (ComponentRectangleRenderer*)
            getComponentHard(sliderRoot, "ComponentRectangleRenderer", NULL);
    rootRect->BorderRadius = 0xffffff;
    ComponentColor *color =
        (ComponentColor*)
            getComponentHard(sliderRoot, "ComponentColor", NULL);
    color->background = (RGBA) {
        .r = 126 / 255.0,
        .g = 126 / 255.0,
        .b = 126 / 255.0,
        .a = 1.0
    };

    JID *dragPart = JIDRectangle(x, y, 12, 12);
    ComponentRectangleRenderer *dragRect =
        (ComponentRectangleRenderer*)
            getComponentHard(dragPart, "ComponentRectangleRenderer", NULL);
    dragRect->BorderRadius = 0xffffff;
    ComponentRelativeTransform *rt = componentRelativeTransform();
    rt->y = -(DRAGGER_SIZE / 3.0);
    rt->x = 0.0f;
    rt->height = DRAGGER_SIZE;
    rt->width = DRAGGER_SIZE;
    JIDAddComp(dragPart, (JIDComponent*)rt);

    color = (ComponentColor*) getComponentHard(dragPart, "ComponentColor", NULL);
    color->background = (RGBA){
        .r = 120 / 255.0f,
        .g = 75 / 255.0f,
        .b = 236 / 255.0f,
        .a = 1.0,
    };

    ComponentEventHandler *ceh = componentEventHandler();
    ceh->onClick = sliderMouseDown;
    
    JIDAddComp(dragPart, (JIDComponent*) ceh);

    JIDSetParent(dragPart, sliderRoot);

    return sliderRoot;
}

int JIDAddComp(JID *jid, JIDComponent *comp) {
    if (jid->ComponentCount >= MAX_COMPONENTS) {
        fprintf(stderr,
            "JID `%s` has reached it's component cap.",
            jid->JIDType);
        exit(1);
        return 1;
    }
    if (JIDFindComp(jid, comp->ComponentName) != -1) {
        fprintf(stderr,
            "JID `%s` already has component `%s`.",
            jid->JIDType, comp->ComponentName);
        return 1;
    }
    jid->Components[jid->ComponentCount] = comp;
    jid->ComponentCount++;
    return 0;
}

int JIDFindComp(JID *jid, const char *compName) {
    for (size_t i = 0; i < jid->ComponentCount; i++) {
        if (!strcmp(jid->Components[i]->ComponentName, compName)) {
            return (int)i;
        }
    }
    return -1;
}

ComponentColor *componentColorBG(float r, float g, float b, float a) {
    ComponentColor *color = scalloc(sizeof(ComponentColor));
    color->ComponentName = "ComponentColor";
    // color->background = (RGBA*)scalloc(sizeof(RGBA));
    color->background.r = r;
    color->background.g = g;
    color->background.b = b;
    color->background.a = a;
    // color->foreground = (RGBA*)scalloc(sizeof(RGBA));
    color->foreground.r = 0.0;
    color->foreground.g = 0.0;
    color->foreground.b = 0.0;
    color->foreground.a = 0.0;
    return color;
}
ComponentColor *componentColorFG(float r, float g, float b, float a) {
    ComponentColor *color = scalloc(sizeof(ComponentColor));
    color->ComponentName = "ComponentColor";
    color->isLayout = false;
    color->isRenderable = false;
    // color->background = (RGBA*)scalloc(sizeof(RGBA));
    color->background.r = 0.0;
    color->background.g = 0.0;
    color->background.b = 0.0;
    color->background.a = 0.0;
    // color->foreground = (RGBA*)scalloc(sizeof(RGBA));
    color->foreground.r = r;
    color->foreground.g = g;
    color->foreground.b = b;
    color->foreground.a = a;
    return color;
}

ComponentVBoxLayout *componentVBoxLayout(float spacing, Padding padding) {
    ComponentVBoxLayout *vboxLayout = scalloc(sizeof(ComponentVBoxLayout));
    vboxLayout->isLayout = true;
    vboxLayout->isRenderable = false;
    vboxLayout->Padding = padding;
    vboxLayout->Spacing = spacing;
    vboxLayout->ComponentName = "ComponentVBoxLayout";
    vboxLayout->compute = componentVBoxLayoutCompute;
    vboxLayout->NoAutoSizeX = false;
    vboxLayout->NoAutoSizeY = false;
    return vboxLayout;
}

ComponentHBoxLayout *componentHBoxLayout(float spacing, Padding padding) {
    ComponentHBoxLayout *hboxLayout = scalloc(sizeof(ComponentHBoxLayout));
    hboxLayout->isLayout = true;
    hboxLayout->isRenderable = false;
    hboxLayout->Padding = padding;
    hboxLayout->Spacing = spacing;
    hboxLayout->ComponentName = "ComponentHBoxLayout";
    hboxLayout->compute = componentHBoxLayoutCompute;
    hboxLayout->NoAutoSizeX = false;
    hboxLayout->NoAutoSizeY = false;
    return hboxLayout;
}

ComponentRectangleRenderer *componentRectangleRenderer(void) {
    ComponentRectangleRenderer *renderer = scalloc(sizeof(ComponentRectangleRenderer));
    renderer->ComponentName = "ComponentRectangleRenderer";
    renderer->BorderRadius = 0;
    renderer->isRenderable = true;
    renderer->render = renderRectangleRenderer;
    renderer->isHovering = false;
    return renderer;
}
ComponentRenderDamage componentRenderDamage(void) {
    return (ComponentRenderDamage){
        .ComponentName = "ComponentRenderDamage",
        .IsDamaged = true
    };
}

int JIDSetParent(JID *jid, JID *parent) {
    if (parent == NULL) {
        fprintf(stderr, "Attempt to call `JIDSetParent` with NULL parent\n");
        return 1;
    }
    if (jid == NULL) {
        fprintf(stderr, "Attempt to call `JIDSetParent` with NULL jid\n");
        return 1;
    }
    if (parent->ChildrenCount + 1 > parent->ChildrenAlloc) {
        parent->ChildrenAlloc += 128;
        parent->Children = realloc(parent->Children, sizeof(JID*) * parent->ChildrenAlloc); // !
    }
    parent->Children[parent->ChildrenCount++] = jid;
    jid->Parent = parent;
    return 0;
}

bool ComponentNameEq(JIDComponent *comp, const char *expect) {
    return !strcmp(comp->ComponentName, expect);
}

// Deleting

void ComponentColorFree(ComponentColor *color) {
    assert(ComponentNameEq((JIDComponent*)color, "ComponentColor"));

    // free(color->foreground);
    // free(color->background);
    // free((char*)color->ComponentName);
    free(color);
}

void ComponentTextRendererFree(ComponentTextRenderer *tr) {
    assert(ComponentNameEq((JIDComponent*)tr, "ComponentTextRenderer"));
    // free(tr->Text);
    // free((char*)tr->ComponentName);
    free(tr);
}

void JIDDestroy(JID *jid) {
    if (jid == NULL) {
        fprintf(stderr, "Attempt to free a NULL jid");
        exit(1);
    }
    // for (size_t i = 0; i < jid->ChildrenCount; i++) {
    //     JIDDestroy(jid->Children[i]);
    // }
    for (size_t i = 0; i < jid->ComponentCount; i++) {
        if (0){//ComponentNameEq(jid->Components[i], "ComponentColor")) {
        //     ComponentColorFree((ComponentColor*)jid->Components[i]);
        // } else if (ComponentNameEq(jid->Components[i], "ComponentTextRenderer")) {
        //     ComponentTextRendererFree((ComponentTextRenderer*)jid->Components[i]);
        } else {
            // free(jid->Components[i]->ComponentName);
            free(jid->Components[i]);
        }
    }
    // free(jid->Components);
    // free((char*)jid->JIDType);
    free(jid->Children);
    if (jid->Parent != NULL) {
        jid->Parent->ChildrenCount--;
    }
    jid->Parent = NULL;
    free(jid);
}
