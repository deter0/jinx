// https://github.com/jamesroutley/write-a-hash-table

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "../core/jid.h"

// typedef struct ht_item {
//     char *key;
//     char *value;
// } ht_item;

typedef struct Variable {
    char *type;
    RGBA colorValue;
    int intValue;
    float floatValue;
} Variable;

typedef struct HashTableItem {
    char *key;
    Variable *value;
} HashTableItem;

// typedef struct ht_hash_table {
//     size_t count;
//     size_t allocated;
//     size_t base_size;
//     ht_item **items;
// } ht_hash_table;

typedef struct HashTable {
    size_t count;
    size_t allocated;
    size_t base_size;
    HashTableItem **items;
} HashTable;

int validateKey(char *key);
int ht_hash(const char* s, const size_t a, const size_t m);

HashTable *ht_new(void);
HashTableItem *ht_new_item(const char *k, Variable *var);
void ht_del_item(HashTableItem *item);
void ht_del_hash_table(HashTable *ht);

void ht_insert(HashTable* ht, const char* key, Variable *var);
Variable* ht_search(HashTable* ht, const char* key);
void ht_delete(HashTable* h, const char* key);
void log_ht(HashTable *ht, FILE *fd);
