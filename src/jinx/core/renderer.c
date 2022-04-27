#include <cairo/cairo.h>
#include <stdio.h>
#include <stdlib.h>

#include "src/jinx/core/jid.h"

void renderRoot(JID *rootJID, cairo_t *ctx) {
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
        renderRoot(rootJID->Children[i], ctx);
    }
}
