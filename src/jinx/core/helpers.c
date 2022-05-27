#include "src/jinx/core/jid.h"
#include "src/jinx/core/helpers.h"

#include <assert.h>
#include <string.h>

int JIDSetBGColor(JID *jid, RGBA new) {
    ComponentColor *color = (ComponentColor*)getComponentSoft(jid,
                                                            "ComponentColor",
                                                            "JIDSetBGColor");
    ComponentRenderDamage *damage = (ComponentRenderDamage*)getComponentHard(jid,
                                                            "ComponentRenderDamage",
                                                            NULL);
    if (color == NULL)
        return 1;
    color->background = new;
    if (damage != NULL)
        damage->IsDamaged = true;

    return 0;
}

int JIDSetFGColor(JID *jid, RGBA new) {
    ComponentColor *color = (ComponentColor*)getComponentSoft(jid,
                                                            "ComponentColor",
                                                            "JIDSetFGColor");
    ComponentRenderDamage *damage = (ComponentRenderDamage*)getComponentHard(jid,
                                                            "ComponentRenderDamage",
                                                            NULL);
    if (color == NULL)
        return 1;
    if (damage != NULL)
        damage->IsDamaged = true;
    color->foreground = new;
    return 0;
}

int JIDSetText(JID *jid, char *text) {
    ComponentTextRenderer *textComp = (ComponentTextRenderer*)getComponentSoft(jid,
                                                            "ComponentTextRenderer",
                                                            "JIDSetText");
    ComponentRenderDamage *damage = (ComponentRenderDamage*)getComponentHard(jid,
                                                            "ComponentRenderDamage",
                                                            NULL);
    if (textComp == NULL)
        return 1;
    if (damage != NULL)
        damage->IsDamaged = true;
    textComp->Text = strdup(text);
    return 0;
}

ComponentTransform *GetTransform(JID *jid) {
    ComponentTransform *transform = (ComponentTransform*)getComponentHard(jid, "ComponentTransform", NULL);
    return transform;
}

int SetSize(JID *jid, float width, float height) {
    ComponentTransform *tf = GetTransform(jid);
    tf->width = width;
    tf->height = height;
    return 0;
}

int SetPosition(JID *jid, float x, float y) {
    ComponentTransform *tf = GetTransform(jid);
    tf->x = x;
    tf->y = y;
    return 0;
}

JID *FindRoot(JID *jid) {
    JID *current = jid;
    while (strcmp(current->JIDType, "Root") != 0) {
        if (current->Parent == NULL) {
            fprintf(stderr, "FindRoot ran into NULL parent!\n");
            return NULL;
        }
        current = current->Parent;
    }
    return current;
}

bool TransformIntersectsTransform(ComponentTransform *a, ComponentTransform *b) {
    assert(a != NULL && b != NULL);
    bool intersects = false;
    
    float AVertices[4][2] = {
        {a->x,            a->y},            // Top left
        {a->x + a->width, a->y},            // Top right
        {a->x,            a->y + a->height}, // Bottom left
        {a->x + a->width, a->y + a->height}  // Bottom right
    };
    for (size_t i = 0; i < sizeof(AVertices) / sizeof(AVertices[0]); i++) {
        if (AVertices[i][0] >= b->x && AVertices[i][0] <= b->x + b->width) {
            if (AVertices[i][1] >= b->y && AVertices[i][1] <= b->y + b->height) {
                intersects = true;
            }
        }
    }

    return intersects;// || TransformIntersectsTransform(b, a);
}

typedef struct JIDPool {
    size_t Allocated;
    size_t Count;
    JID **JIDs;
} JIDPool;

JIDPool *NewJIDPool(size_t initialAllocation) {
    if (initialAllocation == 0) {
        initialAllocation = 50;
    }
    JIDPool *pool = calloc(1, sizeof(JIDPool));
    if (pool == NULL) {
        fprintf(stderr, "Error allocating JID Pool\n");
        exit(1);
    }
    pool->Allocated = initialAllocation;
    pool->JIDs = calloc(pool->Allocated, sizeof(JID*));
    if (pool->JIDs == NULL) {
        fprintf(stderr, "Error allocating jids inside JIDPool of length: %zu", pool->Allocated);
        exit(1);
    }
    return pool;
}

void JIDPoolAdd(JIDPool *pool, JID *toAdd) {
    assert(pool != NULL);
    assert(toAdd != NULL);

    if (pool->Count + 1 >= pool->Allocated) {
        pool->Allocated *= 2;
        pool->JIDs = realloc(pool->JIDs, pool->Allocated);
        if (pool->JIDs == NULL) {
            fprintf(stderr, "Error reallocating jids of JidPool to size %zu\n", pool->Allocated);
            exit(1);
        }
    }
    pool->JIDs[pool->Count++] = toAdd;
}

/**
 * Delete's a JIDPool
 *
 @param DeleteJIDs - whether to delete the JIDs or only the pool itself
*/
void JIDPoolDelete(JIDPool *pool, bool DeleteJIDs) {
    assert(pool != NULL);
    if (DeleteJIDs) {
        for (size_t i = 0; i < pool->Allocated; i++) {
            JIDDestroy(&pool->JIDs[i]);
        }
    }
    free(pool->JIDs);
    free(pool);
}

void FindInRegionHelper(JID *current,
                        ComponentTransform *searchAreaTF,
                        JIDPool *inRegion) {
    ComponentTransform *tf = GetTransform(current);
    assert(tf != NULL);
    if (TransformIntersectsTransform(searchAreaTF, tf)) {
        JIDPoolAdd(inRegion, current);
    }
    for (size_t i = 0; i < current->ChildrenCount; i++) {
        FindInRegionHelper(current->Children[i], searchAreaTF, inRegion);
    }
}

JIDPool *FindInRegion(JID *root, float x, float y, float w, float h) {
    JIDPool *inRegion = NewJIDPool(50);
    ComponentTransform *searchAreaTF = componentTransform(x, y, w, h);
    // for (size_t i = 0; i < root->ChildrenCount; i++) {
    //     FindInRegionHelper(root->Children[i], searchAreaTF, inRegion);
    // }
    FindInRegionHelper(root, searchAreaTF, inRegion);
    DestroyComponent((JIDComponent*)searchAreaTF);
    return inRegion;
}

int Damage(JID *jid) {
    JID *root = FindRoot(jid);
    ComponentTransform *tf = GetTransform(jid);
    JIDPool *inRegion = FindInRegion(root, tf->x, tf->y, tf->width, tf->height);
    for (size_t i = 0; i < inRegion->Count; i++) {
        ComponentRenderDamage *damage = (ComponentRenderDamage*)getComponentHard(jid, "ComponentRenderDamage", "Damage");
        damage->IsDamaged = true;    
    }
    JIDPoolDelete(inRegion, false);

    return 0;
}
