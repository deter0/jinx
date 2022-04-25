#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <cairo/cairo.h>

#define MAX_COMPONENTS 128

/* Jinx Widget */
typedef struct JID {
    const char *JIDType;
    struct JIDComponent *Components[MAX_COMPONENTS];
    size_t ComponentCount;

    struct JID **Children;
    size_t ChildrenCount;
    size_t ChildrenAlloc;

    struct JID *Parent;
} JID;

#define ExtendJIDComponent const char *ComponentName; \
                               bool isRenderable;

typedef struct JIDComponent {
    ExtendJIDComponent;
} JIDComponent;

#define ExtendJIDRenderableComp ExtendJIDComponent;\
                            void(*render)(JID *jid, cairo_t *ctx);
// Extends `JIDComponent`
typedef struct ComponentRenderable {
    ExtendJIDRenderableComp;
} ComponentRenderable;

// Extends `JIDComponentRenderable`
typedef struct ComponentRectangleRenderer {
    ExtendJIDRenderableComp;
    float BorderRadius;
} ComponentRectangleRenderer;
ComponentRectangleRenderer *componentRectangleRenderer(void);

typedef struct ComponentRenderDamage {
    const char *ComponentName;
    bool IsDamaged;
} ComponentRenderDamage;
ComponentRenderDamage componentRenderDamage(void);

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

int JIDSetParent(JID *jid, JID *parent);

// Extends `JIDComponent`
typedef struct ComponentTransform {
    const char *ComponentName;
    float x;
    float y;
    float width;
    float height;
} ComponentTransform;
ComponentTransform *componentTransform(float x, float y, float w, float h);

typedef struct RGBA {
    float r;
    float g;
    float b;
    float a;
} RGBA;

// Extends `JIDComponent`
typedef struct ComponentColor {
    ExtendJIDComponent;
    RGBA *foreground;
    RGBA *background;
} ComponentColor;
ComponentColor componentColorFG(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
ComponentColor *componentColorBG(float r, float g, float b, float a);
ComponentColor componentColorFGBG(
    unsigned char fr, unsigned char fg, unsigned char fb, unsigned char fa,
    unsigned char br, unsigned char bg, unsigned char bb, unsigned char ba);

static JIDComponent *getComponentHard(JID *jid, const char *target, const char *from);
static JIDComponent *getComponentSoft(JID *jid, const char *target, const char *from);

// Root Component
JID *JIDRoot(float width, float height);
JID *JIDRectangle(float x, float y, float width, float height);
