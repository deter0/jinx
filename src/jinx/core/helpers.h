#pragma once
#include "src/jinx/core/jid.h"

/**
    * Sets the background color
    *
    * @return - Returns 1 if no color comp was found, 0 for success
*/
int JIDSetBGColor(JID *jid, RGBA new);

/**
    * Sets the foreground color
    *
    * @return - Returns 1 if no color comp was found, 0 for success
*/
int JIDSetFGColor(JID *jid, RGBA new);

/**
    * Sets the text if jid has ComponentTextRenderer
    *
    * @return - Returns 1 if no ComponentTextRenderer was found, 0 for success
*/
int JIDSetText(JID *jid, char *text);