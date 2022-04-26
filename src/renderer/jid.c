// JID: Jinx Widget

#include "jid.h"
#include "renderer.h"
#include "render_functions.c"

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

static JIDComponent *getComponentHard(JID *jid, const char *target, const char *from) {
    int index = JIDFindComp(jid, target);
    if (index == -1) {
        fprintf(stderr, "Component: `%s` requires component: `%s`\n", from,
            target);
        exit(1);
    }
    return jid->Components[index];
}
static JIDComponent *getComponentSoft(JID *jid, const char *target, const char *from) {
    int index = JIDFindComp(jid, target);
    if (index == -1) {
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
    return transform;
}

ComponentTextRenderer *componentTextRenderer(void) {
    ComponentTextRenderer *rndr = (ComponentTextRenderer*)scalloc(sizeof(ComponentTextRenderer));
    rndr->ComponentName = "ComponentTextRenderer";
    rndr->FontSize = 24;
    rndr->Text = "";
    rndr->render = renderTextRenderer;
    rndr->isRenderable = true;
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

    return rect;
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
    color->background = (RGBA*)scalloc(sizeof(RGBA));
    color->background->r = r;
    color->background->g = g;
    color->background->b = b;
    color->background->a = a;
    color->foreground = (RGBA*)scalloc(sizeof(RGBA));
    color->foreground->r = 0.0;
    color->foreground->g = 0.0;
    color->foreground->b = 0.0;
    color->foreground->a = 0.0;
    return color;
}
ComponentColor *componentColorFG(float r, float g, float b, float a) {
    ComponentColor *color = scalloc(sizeof(ComponentColor));
    color->ComponentName = "ComponentColor";
    color->background = (RGBA*)scalloc(sizeof(RGBA));
    color->background->r = 0.0;
    color->background->g = 0.0;
    color->background->b = 0.0;
    color->background->a = 0.0;
    color->foreground = (RGBA*)scalloc(sizeof(RGBA));
    color->foreground->r = r;
    color->foreground->g = g;
    color->foreground->b = b;
    color->foreground->a = a;
    return color;
}


ComponentRectangleRenderer *componentRectangleRenderer(void) {
    ComponentRectangleRenderer *renderer = scalloc(sizeof(ComponentRectangleRenderer));
    renderer->ComponentName = "ComponentRectangleRenderer";
    renderer->BorderRadius = 0;
    renderer->isRenderable = true;
    renderer->render = renderRectangleRenderer;
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
