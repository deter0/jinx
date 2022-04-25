#pragma once
#include <stdlib.h>
#include <stdbool.h>

#define MAX_COMPONENTS 128

typedef struct JIDComponent {
    const char *ComponentName;
} JIDComponent;

/* Jinx Widget */
typedef struct JID {
    const char *JIDType;
    JIDComponent *Components[MAX_COMPONENTS];
    size_t ComponentCount;

    struct JID **Children;
    size_t ChildrenCount;
    size_t ChildrenAlloc;
} JID;

/**
 * Add component to a jid
 *
 @param jid - the jid
 @param comp - the component
*/
int JIDAddComp(JID *jid, JIDComponent *comp);

/**
 * Compares the widget type to the expected 
 *
 * @return returns true if matches
*/
bool JIDTypeEq(JID *jid, const char *expect);

/**
 * Find's a component within a JID
 *
 * @return The index of the component or -1 if no component is found
*/
int JIDFindComp(JID *jid, const char *compName);

typedef struct ComponentTransform {
    const char *ComponentName;
    float x;
    float y;
    float width;
    float height;
} ComponentTransform;
ComponentTransform componentTransform(float x, float y, float w, float h);

// Root Component
JID JIDRoot(float width, float height);
