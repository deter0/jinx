// https://github.com/jamesroutley/write-a-hash-table

#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef struct ht_item {
    char *key;
    char *value;
} ht_item;

typedef struct ht_hash_table {
    size_t count;
    size_t allocated;
    size_t base_size;
    ht_item **items;
} ht_hash_table;

int validateKey(char *key);
int ht_hash(const char* s, const size_t a, const size_t m);

ht_hash_table *ht_new(void);
ht_item *ht_new_item(const char *k, const char *v);
void ht_del_item(ht_item *item);
void ht_del_hash_table(ht_hash_table *ht);

void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
void ht_delete(ht_hash_table* h, const char* key);
void log_ht(ht_hash_table *ht, FILE *fd);
