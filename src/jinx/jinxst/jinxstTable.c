#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "jinxstTable.h"


/*
 * Return whether x is prime or not
 *
 * Returns:
 *   1  - prime
 *   0  - not prime
 *   -1 - undefined (i.e. x < 2)
 */
static int is_prime(const size_t x) {
    if (x < 2) { return -1; }
    if (x < 4) { return 1; }
    if ((x % 2) == 0) { return 0; }
    for (size_t i = 3; i <= floor(sqrt((double) x)); i += 2) {
        if ((x % i) == 0) {
            return 0;
        }
    }
    return 1;
}


/*
 * Return the next prime after x, or x if x is prime
 */
static size_t next_prime(size_t x) {
    while (is_prime(x) != 1) {
        x++;
    }
    return x;
}

ht_item *ht_new_item(const char *k, const char *v) {
    ht_item *item = malloc(sizeof(ht_item));
    assert(item != NULL);

    item->key = strdup(k);
    item->value = strdup(v);
    return item;
}

static const char validChars[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHJIKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
int validateKey(char *key) {
    if (strspn(key, validChars) == strlen(key))
        return 1;
    return 0;
}

int ht_hash(const char* s, const size_t a, const size_t m) {
    long hash = 0;
    const size_t len_s = strlen(s);
    for (size_t i = 0; i < len_s; i++) {
        hash += (long)pow(a, len_s - (i+1)) * s[i];
        hash = hash % m;
    }
    return (int)hash;
}

static ht_hash_table* ht_new_sized(const size_t base_size) {
    ht_hash_table* ht = malloc(sizeof(ht_hash_table));
    assert(ht != NULL);
    ht->base_size = base_size;

    ht->allocated = next_prime(ht->base_size);

    ht->count = 0;
    ht->items = calloc((size_t)ht->allocated, sizeof(ht_item*));
    assert(ht->items != NULL);
    return ht;
}

#define HT_INITIAL_BASE_SIZE 53
static ht_item HT_DELETED_ITEM = {NULL, NULL};

ht_hash_table* ht_new(void) {
    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

static void ht_resize(ht_hash_table *ht, const size_t base_size) {
    if (base_size < HT_INITIAL_BASE_SIZE)
        return;
    ht_hash_table *new_ht = ht_new_sized(base_size);
    for (size_t i = 0; i < ht->allocated; i++) {
        ht_item *item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            ht_insert(new_ht, item->key, item->value);
        }
    }

    ht->base_size = new_ht->base_size;
    ht->count = new_ht->count;

    // To delete new_ht, we give it ht's size and items 
    const size_t tmp_size = ht->allocated;
    ht->allocated = new_ht->allocated;
    new_ht->allocated = tmp_size;

    ht_item** tmp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;

    ht_del_hash_table(new_ht);
}

static void ht_resize_up(ht_hash_table* ht) {
    const size_t new_size = ht->base_size * 2;
    ht_resize(ht, new_size);
}


static void ht_resize_down(ht_hash_table* ht) {
    const size_t new_size = ht->base_size / 2;
    ht_resize(ht, new_size);
}

void ht_del_item(ht_item *item) {
    free(item->key);
    free(item->value);
    free(item);
}

void ht_del_hash_table(ht_hash_table* ht) {
    for (size_t i = 0; i < ht->allocated; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL) {
            ht_del_item(item);
        }
    }
    free(ht->items);
    free(ht);
}

#define HT_PRIME_1 151
#define HT_PRIME_2 163

static int ht_get_hash(
    const char* s, const size_t num_buckets, const size_t attempt
) {
    const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
    const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
    return (hash_a + ((int)attempt * (hash_b + 1))) % (int)num_buckets;
}

void ht_insert(ht_hash_table *ht, const char *key, const char *value) {
    const size_t load = ht->count * 100 / ht->allocated;
    if (load > 70) {
        ht_resize_up(ht);
        printf("Resized hash table (+), %zu\n", ht->allocated);
    }
    ht_item *item = ht_new_item(key, value);
    int index = ht_get_hash(item->key, ht->allocated, 0);
    ht_item *cur_item = ht->items[index];
    size_t i = 1;
    while (cur_item != NULL) { // FXXMEEEE: could easily get exhausted
        if (cur_item != &HT_DELETED_ITEM) {
            if (strcmp(cur_item->key, key) == 0) {
                ht_del_item(cur_item);
                ht->items[index] = item;
                return;
            }
        }
        index = ht_get_hash(item->key, ht->allocated, i);
        i++;
        assert(i <= ht->allocated);
    }
    ht->items[index] = item;
    ht->count++;
}

char *ht_search(ht_hash_table *ht, const char *key) {
    int index = ht_get_hash(key, ht->allocated, 0);
    ht_item *item = ht->items[index];
    size_t i = 1;
    // while (item != NULL && item->key != NULL) {
    //     if (strcmp(item->key, key) == 0 && item != &HT_DELETED_ITEM) {
    //         return item->value;
    //     }
    //     index = ht_get_hash(key, ht->allocated, i);
    //     item = ht->items[index];
    //     i++;
    //     assert(i <= ht->allocated);
    // }
    return NULL;
}

void ht_delete(ht_hash_table *ht, const char *key) {
    const size_t load = ht->count * 100 / ht->allocated;
    if (load < 10) {
        ht_resize_down(ht);
        printf("Resized hash table (-) %zu\n", ht->allocated);
    }
    int index = ht_get_hash(key, ht->allocated, 0);
    ht_item *item = ht->items[index];
    size_t i = 1;
    while (item != NULL) {
        if (item != &HT_DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {
                ht_del_item(item);
                ht->items[index] = &HT_DELETED_ITEM;
                ht->count--;
            }
        }
        index = ht_get_hash(key, ht->allocated, i);
        item = ht->items[index];
        i++;
        assert(i <= ht->allocated);
    }
}

#if 1

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    int hash_test = ht_get_hash("cat", 53, 0);
    printf("%d\n", hash_test);
    assert(hash_test == 5);

    int validKeyResult = validateKey("\"The quick brown fox,. jumped over the lazy dog!?;\"");
    assert(validKeyResult == 1);
    int invalidKeyResult = validateKey("¥©«®¶");
    assert(invalidKeyResult == 0);

    ht_hash_table *ht = ht_new();

    ht_insert(ht, "cat", "charlie");
    assert(strcmp(ht_search(ht, "cat"), "charlie") == 0);
    assert(ht_search(ht, "dog") == NULL);
    ht_insert(ht, "dog", "pepsi");
    assert(strcmp(ht_search(ht, "dog"), "pepsi") == 0);
    assert(strcmp(ht_search(ht, "cat"), "charlie") == 0);
    ht_delete(ht, "dog"); // Should resize
    assert(ht_search(ht, "dog") == NULL);
    assert(strcmp(ht_search(ht, "cat"), "charlie") == 0);

    ht_del_hash_table(ht);

    for (int i = 1; i < argc; i++) {
        printf("%s\n", argv[i]);
        /*switch (argv[i][0]) {
            case '+': {
                assert(i + 1 <= argc);
                char *value = argv[i + 1];
                ht_insert(ht, argv[i] + 1, value);
                printf("Added ht[%s] = %s\n", argv[i] + 1, value);
            } break;
            case '-': {
                ht_delete(ht, argv[i] + 1);
                printf("Deleted ht[%s]\n", argv[i] + 1);
            } break;
            case '=': {
                assert(i + 1 <= argc);
                char *value = argv[i + 1];
                char *key = argv[i] + 1;
                if (!strcmp(ht_search(ht, key), value)) {
                    fprintf(stderr, "ht[%s] == %s Failed!, ht[%s] = %s", key, value, key, ht_search(ht, key));
                } else {
                    printf("Checked!\n");
                }
            } break;
            default: {
                fprintf(stderr, "Unknown prefix: %c\n", argv[i][0]);
                exit(1);
            }
        }*/
    }

    return 0;
}

#endif
