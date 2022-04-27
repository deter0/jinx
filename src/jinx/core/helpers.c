#include "src/jinx/core/jid.h"
#include "src/jinx/core/helpers.h"

int JIDSetBGColor(JID *jid, RGBA new) {
    ComponentColor *color = (ComponentColor*)getComponentSoft(jid,
                                                            "ComponentColor",
                                                            "JIDSetBGColor");
    if (color == NULL)
        return 1;
    color->background = new;
    return 0;
}

int JIDSetFGColor(JID *jid, RGBA new) {
    ComponentColor *color = (ComponentColor*)getComponentSoft(jid,
                                                            "ComponentColor",
                                                            "JIDSetFGColor");
    if (color == NULL)
        return 1;
    color->foreground = new;
    return 0;
}

int JIDSetText(JID *jid, char *text) {
    ComponentTextRenderer *textComp = (ComponentTextRenderer*)getComponentSoft(jid,
                                                            "ComponentTextRenderer",
                                                            "JIDSetText");
    if (textComp == NULL)
        return 1;
    textComp->Text = text;
    return 0;
}
