#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "layouts.h"
#include "jid.h"

void componentVBoxLayoutCompute(JID *self) {
    printf("!!\n");
    ComponentVBoxLayout *layout = (ComponentVBoxLayout*)getComponentHard(self,
                                 "ComponentVBoxLayout", "componentVBoxLayoutCompute");
    ComponentTransform *selfTransform = (ComponentTransform*)getComponentSoft(self,
                                 "ComponentTransform", "componentVBoxLayoutCompute");
    if (selfTransform == NULL) {
        fprintf(stderr, "It is recommended for a layout component to be alongside a transform component.\n");
    }
    size_t lastY;
    size_t greatestX = 0;
    if (selfTransform) {
        lastY = selfTransform->y + layout->Padding.top;
    } else {
        lastY = layout->Padding.top;
    }
    for (size_t i = 0; i < self->ChildrenCount; i++) {
        ComponentTransform *childTransform
            = (ComponentTransform*)getComponentSoft(self->Children[i], "ComponentTransform", "componentVBoxLayoutCompute");
        if (childTransform != NULL) {
            childTransform->y = lastY;
            if (selfTransform != NULL) {
                childTransform->x = selfTransform->x + layout->Padding.left;
            }
            lastY = childTransform->y + childTransform->height + layout->Spacing;
            if (childTransform->x + childTransform->width > greatestX) {
                greatestX = childTransform->width + childTransform->x;
            }
        }
    }
    lastY += layout->Padding.bottom;
    lastY -= layout->Spacing;
    if (selfTransform) {
        if (!layout->NoAutoSizeY)
            selfTransform->height = lastY;
        if (!layout->NoAutoSizeX)
            selfTransform->width = greatestX - selfTransform->x + layout->Padding.right;
    }
}


void componentHBoxLayoutCompute(JID *self) {
    printf("!!\n");
    ComponentHBoxLayout *layout = (ComponentHBoxLayout*)getComponentHard(self,
                                 "ComponentHBoxLayout", "componentHBoxLayoutCompute");
    ComponentTransform *selfTransform = (ComponentTransform*)getComponentSoft(self,
                                 "ComponentTransform", "componentHBoxLayoutCompute");
    size_t lastX;
    size_t greatestY = 0;
    if (selfTransform) {
        lastX = selfTransform->x + layout->Padding.left;
    } else {
        lastX = layout->Padding.left;
    }
    for (size_t i = 0; i < self->ChildrenCount; i++) {
        ComponentTransform *childTransform
            = (ComponentTransform*)getComponentSoft(self->Children[i], "ComponentTransform", "componentVBoxLayoutCompute");
        if (childTransform != NULL) {
            childTransform->x = lastX;
            if (selfTransform != NULL) {
                childTransform->y = selfTransform->y + layout->Padding.top;
            }
            lastX = childTransform->x + childTransform->width + layout->Spacing;
            if (childTransform->y + childTransform->height > greatestY) {
                greatestY = childTransform->y + childTransform->height;
            }
        }
    }
    lastX += layout->Padding.right;
    lastX -= layout->Spacing;
    if (selfTransform) {
        if (!layout->NoAutoSizeY)
            selfTransform->height = lastX;
        if (!layout->NoAutoSizeX)
            selfTransform->height = greatestY - selfTransform->y + layout->Padding.bottom;
    }
}
