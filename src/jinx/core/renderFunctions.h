#pragma once

#include "src/jinx/core/jid.h"
#include "src/jinx/core/renderer.h"
#include "cairo/cairo.h"

#include <stdio.h>

void renderRectangleRenderer(JID *jid, cairo_t *ctx);
void renderTextRenderer(JID *jid, cairo_t *ctx);