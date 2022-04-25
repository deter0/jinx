// JID: Jinx Widget

#include "jid.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

// Components

ComponentTransform componentTransform(float x, float y, float w, float h) {
    return (ComponentTransform){
        .ComponentName = "ComponentTransform",
        .x = x,
        .y = y,
        .width = w,
        .height = h
    };
}

////

bool JIDTypeEq(JID *jid, const char *expect) {
    return !strcmp(jid->JIDType, expect);
}

JID JIDRoot(float width, float height) {
    JID root = {
        .JIDType ="Root",

        .Children = NULL,
        .ChildrenCount = 0,
        .ChildrenAlloc = 0,

        .Components = NULL,
        .ComponentCount = 0,
    };
    ComponentTransform transform = componentTransform(0, 0, width, height);
    JIDAddComp(&root, (JIDComponent*)&transform);
    return root;
}

int JIDAddComp(JID *jid, JIDComponent *comp) {
    if (jid->ComponentCount >= MAX_COMPONENTS) {
        fprintf(stderr,
            "JID `%s` has reached it's component cap.",
            jid->JIDType);
        return 1;
    }
    if (JIDFindComp(jid, comp->ComponentName)) {
        fprintf(stderr,
            "JID `%s` already has component `%s`.",
            jid->JIDType, comp->ComponentName);
        return 1;
    }
    jid->Components[jid->ComponentCount] = comp;
    jid->ComponentCount++;
    return 0;
}

int JIDFindComp(JID *jid, const char *compName) {
    for (size_t i = 0; i < jid->ComponentCount; i++) {
        if (jid->Components[i]->ComponentName == compName) {
            return i;
        }
    }
    return -1;
}
