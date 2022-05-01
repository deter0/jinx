#include <cairo/cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "src/jinx/core/jid.h"

static void findAlone(JID *jid, JID **alone, size_t *aloneAlloc, size_t *aloneLen) {
    for (size_t i = 0; i < jid->ChildrenCount; i++) {
        if (jid->Children[i]->ChildrenCount <= 0) { // Alone
            if (*aloneLen + 1 >= *aloneAlloc) {
                *aloneAlloc = *aloneAlloc * 2;
                alone = (JID**)realloc(alone, sizeof(JID*) * *aloneAlloc);
                assert(alone != NULL);
            }
            alone[*aloneLen] = jid->Children[i];
            *aloneLen += 1;
        } else {
            findAlone(jid->Children[i], alone, aloneAlloc, aloneLen);
        }
    }
}

static void layoutUpdateRecurseUpward(JID *alone, int seed) {
    if (alone == NULL)
        return;
    for (size_t i = 0; i < alone->ComponentCount; i++) {
        if (alone->Components[i] != NULL && alone->Components[i]->isLayout) {
            ComponentLayout *layout = (ComponentLayout*)alone->Components[i];
            if (layout->compute != NULL) {
                if (layout->LastSeed != seed) {
                    layout->compute(alone);
                    layout->LastSeed = seed;
                } else {
                    // printf("Dropped layout update.\n");
                }
            } else {
                fprintf(stderr, "Layout component %s compute function is null.\n", layout->ComponentName);
            }
        }
    }
    if (alone->Parent != NULL) {
        layoutUpdateRecurseUpward(alone->Parent, seed);
    }
}

static void updateLayouts(JID *rootJID) {
    size_t aloneAllocated = 200;
    size_t aloneLength = 0;
    JID **alone = malloc(sizeof(JID*) * aloneAllocated);
    assert(alone != NULL);
    int seed = rand();
    findAlone(rootJID, alone, &aloneAllocated, &aloneLength);
    for (size_t i = 0; i < aloneLength ; i++) {
        if (alone[i]->JIDType != NULL)
            layoutUpdateRecurseUpward(alone[i], seed);
    }
}

static void renderRootRecursive(JID *rootJID, cairo_t *ctx) {
    for (size_t i = 0; i < rootJID->ComponentCount; i++) {
        if (rootJID->Components[i]->isRenderable) {
            ComponentRenderable *rend = (ComponentRenderable*)rootJID->Components[i];
            if (rend->render != NULL) {
                rend->render(rootJID, ctx);
            } else {
                fprintf(stderr, "Renderable component, `%s`, render function is NULL.\n", rootJID->Components[i]->ComponentName);
            }
        }
    }
    for (size_t i = 0; i < rootJID->ChildrenCount; i++) {
        renderRootRecursive(rootJID->Children[i], ctx);
    }
}

void renderRoot(JID *rootJID, cairo_t *ctx) {
    cairo_rectangle(ctx, 0, 0, 1920, 1080);
    cairo_set_source_rgb(ctx, 20 / 255.0,
                                22 / 255.0,
                                21 / 255.0);
    cairo_fill(ctx);
    updateLayouts(rootJID);
    renderRootRecursive(rootJID, ctx);
}
