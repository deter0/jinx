#pragma once

#include "jid.h"
#include "renderer.h"
#include "cairo/cairo.h"

#include <stdio.h>

static JIDComponent *getComponentHard(JID *jid, const char *target, const char *src) {
    int index = JIDFindComp(jid, target);
    if (index == -1) {
        fprintf(stderr, "Component: `%s` requires component: `%s`\n", src,
            target);
        exit(1);
    }
    return jid->Components[index];
}
static JIDComponent *getComponentSoft(JID *jid, const char *target, const char *src) {
    int index = JIDFindComp(jid, target);
    if (index == -1) {
        fprintf(stderr, "Component `%s` recommends component: `%s`\n", src,
            target);
        return NULL;
    }
    return jid->Components[index];
}

void renderRectangleRenderer(JID *jid, cairo_t *ctx) {
    ComponentTransform *transform = (ComponentTransform*)getComponentHard(
                                            jid,
                                            "ComponentTransform",
                                            "ComponentRectangleRenderer");
    // ComponentRectangleRenderer *rect = (ComponentRectangleRenderer*)getComponentHard(
    //                                         jid, "ComponentRectangleRenderer", "ComponentRectangleRenderer");
    ComponentColor *color = (ComponentColor*)getComponentSoft(
                                            jid,
                                            "ComponentColor",
                                            "ComponentRectangleRenderer");
    cairo_save(ctx);
    if (color != NULL) {
        cairo_set_source_rgba(ctx,
            color->background->r,
            color->background->g,
            color->background->b,
            color->background->a);
        printf("COLOR: %f, %f, %f\n", color->background->r, color->background->g, color->background->b);
    } else {
        cairo_set_source_rgba(ctx, 1.0, 1.0, 1.0, 1.0);
    }
    cairo_rectangle(ctx, transform->x, transform->y, transform->width, transform->height);
    cairo_fill(ctx);
    cairo_restore(ctx);
}
