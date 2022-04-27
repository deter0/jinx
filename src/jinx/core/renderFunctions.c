#pragma once

#include "src/jinx/core/jid.h"
#include "src/jinx/core/renderer.h"
#include "cairo/cairo.h"

#include <stdio.h>

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
            color->background.r,
            color->background.g,
            color->background.b,
            color->background.a);
        // printf("COLOR: %f, %f, %f\n", color->background->r, color->background->g, color->background->b);
    } else {
        cairo_set_source_rgba(ctx, 1.0, 1.0, 1.0, 1.0);
    }
    cairo_rectangle(ctx, transform->x, transform->y, transform->width, transform->height);
    cairo_fill(ctx);
    cairo_restore(ctx);
}


void renderTextRenderer(JID *jid, cairo_t *ctx) {
    ComponentTransform *transform = (ComponentTransform*)getComponentHard(
                                            jid,
                                            "ComponentTransform",
                                            "ComponentTextRenderer");
    ComponentColor *color = (ComponentColor*)getComponentSoft(
                                            jid,
                                            "ComponentColor",
                                            "ComponentTextRenderer");
    ComponentTextRenderer *tren = (ComponentTextRenderer*)getComponentHard(
                                            jid,
                                            "ComponentTextRenderer",
                                            "ComponentTextRenderer");
    cairo_save(ctx);
    if (color != NULL) {
        cairo_set_source_rgba(ctx,
            color->foreground.r,
            color->foreground.g,
            color->foreground.b,
            color->foreground.a);
        printf("Foreground: %f, %f, %f, %f\n",
            color->foreground.r,
            color->foreground.g,
            color->foreground.b,
            color->foreground.a);
    } else {
        cairo_set_source_rgba(ctx, 0.0, 0.0, 0.0, 1.0);
    }
    cairo_set_font_size(ctx, (double)tren->FontSize);
    cairo_move_to(ctx, transform->x, transform->y + tren->FontSize);
    cairo_select_font_face(ctx, "Iosevka",
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_NORMAL);
    cairo_show_text(ctx, tren->Text);
    cairo_restore(ctx);
}
