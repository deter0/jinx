#pragma once

#include "jinxstTable.h"

#define MAX_INHERITS 20
typedef struct Namespace {
    char *NamespaceName;
    ht_hash_table *Properties;
    struct Namespace *Inherits[MAX_INHERITS];
} Namespace;
void jinxst(char *jinxstSrc);
