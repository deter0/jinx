#include "jid.h"
#include "../eventHandler.h"

// FIXME: This function has some strange behaviour? check `JIDOnMouseClickUp`
static void recurseFindTarget(JID *root, float x, float y, JID **currentTarget) {
    ComponentTransform *transform = (ComponentTransform*)getComponentSoft(root, "ComponentTransform", "JIDOnMouseMove");
    if (transform != NULL) {
        if (x >= transform->x && x < transform->x + transform->width) {
            if (y >= transform->y && y < transform->y + transform->height) {
                *currentTarget = root;
                // Dont break because elements can be on top
            }
        }
    }
    for (size_t i = 0; i < root->ChildrenCount; i++) {
        recurseFindTarget(root->Children[i], x, y, currentTarget);
    }
}

JID *lastTarget = NULL;

void JIDOnMouseMove(JID *root, float x, float y, EventHandler *eh) {
    JID *target = NULL;
    recurseFindTarget(root, x, y, &target);
    if (target != NULL) {
        ComponentEventHandler *ceh = (ComponentEventHandler*)getComponentSoft(target, "ComponentEventHandler", NULL);
        if (ceh != NULL) {
            if (lastTarget != target && ceh->onMouseEnter != NULL) {
                ceh->onMouseEnter(target, x, y, eh);
            }
        }
    }
    if (lastTarget != NULL && lastTarget != target) {
        ComponentEventHandler *ceh = (ComponentEventHandler*)getComponentSoft(lastTarget, "ComponentEventHandler", NULL);
        if (ceh != NULL && ceh->onMouseLeave != NULL) {
            ceh->onMouseLeave(lastTarget, x, y, eh);
        }
    }
    lastTarget = target;
}

void JIDOnMouseClick(JID *root, float x, float y, EventHandler *eh) {
    JID *target = NULL;
    recurseFindTarget(root, x, y, &target);
    if (target != NULL) {
        ComponentEventHandler *ceh = (ComponentEventHandler*)getComponentSoft(target, "ComponentEventHandler", NULL);
        if (ceh != NULL) {
            if (ceh->onClick != NULL) {
                ceh->onClick(target, x, y, eh);
                eh->render(eh);
            }
        }
    }
}

void JIDOnMouseClickUp(JID *root, float x, float y, EventHandler *eh) {
    (void)root;
    JID *target = lastTarget; // Using workaround
    // recurseFindTarget(root, x, y, &target); // HERE, seg faults at `getComponentSoft`
    if (target != NULL) {
        ComponentEventHandler *ceh = (ComponentEventHandler*)getComponentSoft(target, "ComponentEventHandler", NULL);
        if (ceh != NULL && ceh->onClickUp != NULL) {
            ceh->onClickUp(target, x, y, eh);
        }
    }
}
