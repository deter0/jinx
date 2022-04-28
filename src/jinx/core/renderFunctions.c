#pragma once

#include "src/jinx/core/jid.h"
#include "src/jinx/core/renderer.h"
#include "cairo/cairo.h"

#include <math.h>
#include <stdio.h>

#include "renderFunctions.h"

static void roundedRectangle(cairo_t *ctx, float x, float y, float w, float h, float corner_radius); // FWD

void renderRectangleRenderer(JID *jid, cairo_t *ctx) {
    ComponentTransform *transform = (ComponentTransform*)getComponentHard(
                                            jid,
                                            "ComponentTransform",
                                            "ComponentRectangleRenderer");
    ComponentRectangleRenderer *rect = (ComponentRectangleRenderer*)getComponentHard(
                                            jid, "ComponentRectangleRenderer", "ComponentRectangleRenderer");
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
    if (rect->BorderRadius > 0.0) {
        roundedRectangle(ctx, transform->x, transform->y, transform->width, transform->height, rect->BorderRadius);
    } else {
        cairo_rectangle(ctx, transform->x, transform->y, transform->width, transform->height);
    }
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
    } else {
        cairo_set_source_rgba(ctx, 0.0, 0.0, 0.0, 1.0);
    }
    cairo_set_font_size(ctx, (double)tren->FontSize);
    cairo_move_to(ctx,
        transform->x + tren->Padding.left * 0.5,
        transform->y + + tren->Padding.top * 0.5 + tren->FontSize);
    cairo_select_font_face(ctx, "Poppins",
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_NORMAL);
    cairo_text_extents_t exts = {0};
    cairo_text_extents(ctx, tren->Text, &exts);
    transform->width = exts.width + exts.x_bearing + (tren->FontSize / 26.0);
    transform->height = exts.height * 2;
    transform->height += tren->Padding.bottom;
    transform->width += tren->Padding.right;
    printf("%f\n", tren->Padding.bottom);

    cairo_show_text(ctx, tren->Text);
    // if (color != NULL) {
    //     if (color->background.a > 0) {
    //         cairo_rectangle(ctx,
    //             transform->x,
    //             transform->y,
    //             transform->width,
    //             transform->height);
    //         cairo_set_source_rgba(ctx,
    //             color->background.r,
    //             color->background.g,
    //             color->background.b,
    //             color->background.a);
    //         cairo_fill(ctx);
    //     }
    // }

    cairo_restore(ctx);
}

// https://www.cairographics.org/samples/rounded_rectangle/
static void roundedRectangle(cairo_t *ctx, float x, float y, float w, float h, float corner_radius) {
    double radius = corner_radius;
    double degrees = M_PI / 180.0;

    cairo_new_sub_path (ctx);
    cairo_arc (ctx, x + w - radius, y + radius, radius, -90 * degrees, 0 * degrees);
    cairo_arc (ctx, x + w - radius, y + h - radius, radius, 0 * degrees, 90 * degrees);
    cairo_arc (ctx, x + radius, y + h - radius, radius, 90 * degrees, 180 * degrees);
    cairo_arc (ctx, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
    cairo_close_path(ctx);
}
