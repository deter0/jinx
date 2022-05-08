#pragma once

#include "jid.h"
#include "../eventHandler.h"

void onMouseMove();
void JIDOnMouseMove(JID *root, float x, float y, EventHandler *eh);
void JIDOnMouseClick(JID *root, float x, float y, EventHandler *eh);
void JIDOnMouseClickUp(JID *root, float x, float y, EventHandler *eh);
