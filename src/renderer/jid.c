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

////

bool JIDTypeEq(JID *jid, const char *expect) {
    return !strcmp(jid->JIDType, expect);
}

JID JIDRoot(float width, float height) {
    JID root = {
        .JIDType ="Root",

        .Children = NULL,
        .ChildrenCount = 0,
        .ChildrenAlloc = 0,

        .Components = {NULL},
        .ComponentCount = 0,
    };
    ComponentTransform *transform = componentTransform(0, 0, width, height);
    assert(JIDAddComp(&root, (JIDComponent*)transform) != 1);
    ComponentColor *color = componentColorBG(1.0, 1.0, 1.0, 1.0);
    assert(JIDAddComp(&root, (JIDComponent*)color) != 1);
    ComponentRectangleRenderer *rectangleRenderer = componentRectangleRenderer();
    assert(JIDAddComp(&root, (JIDComponent*)rectangleRenderer) != 1);
    // ComponentRenderDamage damage = componentRenderDamage();
    // assert(JIDAddComp(&root, (JIDComponent*)&damage) != 1);

    return root;
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

// ComponentColor componentColorFG(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
//     return (ComponentColor){
//         .foreground = (RGBA){
//             .r = r,
//             .g = g,
//             .b = b,
//             .a = a
//         },
//         .background = (RGBA){
//             .r = 0,
//             .g = 0,
//             .b = 0,
//             .a = 0
//         }
//     };
// }
ComponentColor *componentColorBG(float r, float g, float b, float a) {
    ComponentColor *color = scalloc(sizeof(ComponentColor));
    color->ComponentName = "ComponentColor";
    color->foreground = (RGBA*)scalloc(sizeof(RGBA));
    color->foreground->r = 0.0;
    color->foreground->g = 0.0;
    color->foreground->b = 0.0;
    color->foreground->a = 0.0;
    color->background = (RGBA*)scalloc(sizeof(RGBA));
    color->foreground->r = r;
    color->foreground->g = g;
    color->foreground->b = b;
    color->foreground->a = a;
    return color;
}
// ComponentColor componentColorFGBG(unsigned char fr, unsigned char fg, unsigned char fb, unsigned char fa,
//                                     unsigned char br, unsigned char bg, unsigned char bb, unsigned char ba) {
//     return (ComponentColor){
//         .foreground = (RGBA){
//             .r = fr,
//             .g = fg,
//             .b = fb,
//             .a = fa
//         },
//         .background = (RGBA){
//             .r = br,
//             .g = bg,
//             .b = bb,
//             .a = ba
//         }
//     };
// }

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
