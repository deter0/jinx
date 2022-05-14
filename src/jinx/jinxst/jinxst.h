#pragma once

#include "jinxstTable.h"

#define MAX_INHERITS 20
typedef struct Namespace {
    char *NamespaceName;
    HashTable *Properties;
    struct Namespace *Inherits[MAX_INHERITS];
    size_t InheritsLength;
} Namespace;
void jinxst(char *jinxstSrc);

#define MAX_NAMESPACES 512
Namespace namespaces[MAX_NAMESPACES] = {0};
size_t namespaceCount = 0;
Namespace *getNamespace(char *nameSpaceName);

Namespace *globalStyles = NULL;
