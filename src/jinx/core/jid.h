#pragma once
#include "src/jinx/eventHandler.h"

// #include "../jinxst/jinxst.h"

#include <stdlib.h>
#include <stdbool.h>
#include <cairo/cairo.h>

#define MAX_COMPONENTS 128

/* Jinx Widget */
typedef struct JID {
    char *JIDType;
    struct JIDComponent *Components[MAX_COMPONENTS];
    size_t ComponentCount;

    struct JID **Children;
    size_t ChildrenCount;
    size_t ChildrenAlloc;

    const char *ID;

    struct JID *Parent;
} JID;
JID *EmptyJID(const char *JIDType);

#define ExtendJIDComponent     char *ComponentName; \
                               bool isLayout; \
                               bool isRenderable

typedef struct JIDComponent {
    ExtendJIDComponent;
} JIDComponent;

#define ExtendJIDRenderableComp ExtendJIDComponent;\
                                void(*render)(JID *jid, cairo_t *ctx); \
                                bool isHovering
#define ExtendJIDLayoutComp ExtendJIDComponent;\
                            void(*compute)(JID *self); \
                            int LastSeed
// Extends `JIDComponent`
typedef struct ComponentRenderable {
    ExtendJIDRenderableComp;
} ComponentRenderable;
// Extends `JIDComponent`
typedef struct ComponentLayout {
    ExtendJIDLayoutComp;
} ComponentLayout;

// Extends `JIDComponent`
typedef struct ComponentRelativeTransform {
    ExtendJIDComponent;
    float x;
    float y;
    float width;
    float height;
    float percentX;
    float percentY;
    float percentWidth;
    float percentHeight;
} ComponentRelativeTransform;

// Extends `JIDComponent`
typedef struct ComponentSlider {
    ExtendJIDComponent;
    float percentage; // Don't modify directly use `sliderSetPercentage(JID *jid, float percentage)`
    bool draggable;
} ComponentSlider;

// Extends `ComponentLayout`
typedef struct Padding {
    float top;
    float bottom;
    float left;
    float right;
} Padding;

typedef struct ComponentVBoxLayout {
    ExtendJIDLayoutComp;
    Padding Padding;
    float Spacing;
    bool NoAutoSizeX;
    bool NoAutoSizeY;
} ComponentVBoxLayout;
ComponentVBoxLayout *componentVBoxLayout(float spacing, Padding padding);

typedef struct ComponentHBoxLayout {
    ExtendJIDLayoutComp;
    Padding Padding;
    float Spacing;
    bool NoAutoSizeX;
    bool NoAutoSizeY;
} ComponentHBoxLayout;
ComponentHBoxLayout *componentHBoxLayout(float spacing, Padding padding);


// Extends `JIDComponentRenderable`
typedef struct ComponentRectangleRenderer {
    ExtendJIDRenderableComp;
    float BorderRadius;
} ComponentRectangleRenderer;
ComponentRectangleRenderer *componentRectangleRenderer(void);

// Extends `JIDComponentRenderable`
typedef struct ComponentTextRenderer {
    ExtendJIDRenderableComp;
    int FontSize;
    char *Text;
    char *Font;
    bool Bold;
    Padding Padding;
} ComponentTextRenderer;
ComponentTextRenderer *componentTextRenderer(void);


typedef struct ComponentRenderDamage {
    ExtendJIDComponent;
    bool IsDamaged;
} ComponentRenderDamage;
ComponentRenderDamage *componentRenderDamage(void);
void ComponentTextRendererFree(ComponentTextRenderer *tr);

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
 * Compares the component name to the expected
 *
 * @return returns true if matches
*/
bool ComponentNameEq(JIDComponent *comp, const char *expect);

/**
 * Find's a component within a JID
 *
 * @return The index of the component or -1 if no component is found
*/
int JIDFindComp(JID *jid, const char *compName);

int JIDSetParent(JID *jid, JID *parent);

// Extends `JIDComponent`
typedef struct ComponentTransform {
    ExtendJIDComponent;
    float x;
    float y;
    float width;
    float height;
} ComponentTransform;
ComponentTransform *componentTransform(float x, float y, float w, float h);

ComponentRelativeTransform *componentRelativeTransform(void);

typedef struct RGBA {
    float r;
    float g;
    float b;
    float a;
} RGBA;

// Extends `JIDComponent`
typedef struct ComponentColor {
    ExtendJIDComponent;
    RGBA foreground;
    RGBA background;
} ComponentColor;
void ComponentColorFree(ComponentColor *color);

// Extends `JIDComponent`
typedef struct ComponentEventHandler {
    ExtendJIDComponent;
    // General click function
    void (*onClick)(JID *self, float x, float y, EventHandler *eh);
    // More specific click functions
    void (*onClickUp)(JID *self, float x, float y, EventHandler *eh);

    void (*onMouseEnter)(JID *self, float x, float y, EventHandler *eh);
    void (*onMouseLeave)(JID *self, float x, float y, EventHandler *eh);
} ComponentEventHandler;
ComponentEventHandler *componentEventHandler(void);

ComponentColor *componentColorFG(float r, float g, float b, float a);
ComponentColor *componentColorBG(float r, float g, float b, float a);

JIDComponent *getComponentHard(JID *jid, const char *target, const char *from);
JIDComponent *getComponentSoft(JID *jid, const char *target, const char *from);

// Root Component
JID *JIDRoot(float width, float height);
JID *JIDRectangle(float x, float y, float width, float height);
JID *JIDText(float x, float y, const char *text);
JID *JIDTextButton(float x, float y, char *text);
JID *JIDSlider(float x, float y, float w, float h);

void DestroyComponent(JIDComponent *component);
void JIDDestroy(JID **jid);
