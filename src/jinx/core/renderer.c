#include <cairo/cairo.h>
#include <stdio.h>
#include <stdlib.h>

#include "src/jinx/core/jid.h"

static void findAlone(JID *jid, JID **alone, size_t *aloneAlloc, size_t *aloneLen) {
    for (size_t i = 0; i < jid->ChildrenCount; i++) {
        if (jid->Children[i]->ChildrenCount <= 0) { // Alone
            if (aloneLen + 1 >= aloneAlloc) {
                *aloneAlloc = *aloneAlloc * 2;
                alone = realloc(alone, sizeof(JID*) * *aloneAlloc);
            }
            alone[*aloneLen] = jid->Children[i];
            *aloneLen = *aloneLen + 1;
        } else {
            findAlone(jid->Children[i], alone, aloneAlloc, aloneLen);
        }
    }
}

static void layoutUpdateRecurseUpward(JID *alone) {
    for (size_t i = 0; i < alone->ComponentCount; i++) {
        if (alone->Components[i]->isLayout == 1) {
            ComponentLayout *layout = (ComponentLayout*)alone->Components[i];
            if (layout->compute != NULL) {
                printf("Updated layout\n");
                layout->compute(alone);
            } else {
                fprintf(stderr, "Layout component %s compute function is null.\n", layout->ComponentName);
            }
        }
    }
    if (alone->Parent != NULL) {
        layoutUpdateRecurseUpward(alone->Parent);
    }
}

static void updateLayouts(JID *rootJID) {
    JID **alone = malloc(sizeof(JID*) * 200);
    size_t aloneAllocated = 200;
    size_t aloneLength = 0;
    findAlone(rootJID, alone, &aloneAllocated, &aloneLength);
    for (size_t i = 0; i < aloneLength; i++) {
        layoutUpdateRecurseUpward(alone[i]);
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
    updateLayouts(rootJID);
    renderRootRecursive(rootJID, ctx);
}
