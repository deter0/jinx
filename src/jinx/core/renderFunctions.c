#pragma once

#include "./jid.h"
#include "./renderer.h"
#include "../jinxst/jinxst.h"

#include <cairo/cairo.h>

#include <math.h>
#include <stdio.h>
#include <assert.h>

#include "renderFunctions.h"

static void roundedRectangle(cairo_t *ctx, float x, float y, float w, float h, float corner_radius); // FWD

// FIXME: Should this be here?
static void applyTransformEffects(JID *jid, ComponentTransform *transform) {
    assert(jid != NULL);
    assert(transform != NULL);
    ComponentRelativeTransform *rt = (ComponentRelativeTransform*)
                                        getComponentSoft(jid, "ComponentRelativeTransform", NULL);
    if (rt != NULL) {
        if (jid->Parent != NULL) {
            ComponentTransform *parentTransform = (ComponentTransform*)
                getComponentSoft(jid->Parent, "ComponentTransform", NULL);
            if (parentTransform != NULL) {
                transform->x = parentTransform->x + rt->x;
                transform->y = parentTransform->y + rt->y;
                
                transform->width = rt->width;
                transform->height = rt->height;
                
                transform->x += (parentTransform->width * rt->percentX);
                transform->y += (parentTransform->height * rt->percentY);
                
                transform->width += parentTransform->width * rt->percentWidth;
                transform->height += parentTransform->height * rt->percentHeight;
                // printf("Applied transform effects: %f, %f\n", transform->x, transform->y);
            } else {
                fprintf(stderr, "Relative transform on a jid, %s, who's parent doesn't have a transform component.\n", jid->JIDType);    
            }
        } else {
            fprintf(stderr, "Relative transform on a jid, %s, without a parent.\n", jid->JIDType);
        }
    }
}

void renderRectangleRenderer(JID *jid, cairo_t *ctx) {
    ComponentTransform *transform = (ComponentTransform*)getComponentHard(
                                            jid,
                                            "ComponentTransform",
                                            "ComponentRectangleRenderer");
    applyTransformEffects(jid, transform);
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
        float smallestAxis = (transform->width < transform->height ? transform->width : transform->height) / 2.0f;
        roundedRectangle(
            ctx, transform->x,
            transform->y,
            transform->width,
            transform->height,
            rect->BorderRadius > smallestAxis ? smallestAxis : rect->BorderRadius
        );
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
    applyTransformEffects(jid, transform);
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
    cairo_select_font_face(ctx, tren->Font,
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_NORMAL);
    cairo_text_extents_t exts = {0};
    cairo_text_extents(ctx, tren->Text, &exts);
    transform->width = exts.width + exts.x_bearing + (tren->FontSize / 26.0);
    transform->height = tren->FontSize + tren->Padding.bottom * 0.5;
    transform->height += tren->Padding.bottom;
    transform->width += tren->Padding.right;

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
