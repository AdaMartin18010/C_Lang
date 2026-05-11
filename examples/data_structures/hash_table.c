/*
 * 开放寻址法哈希表示例
 * 编译: gcc -Wall -Wextra -O2 -o hash_table hash_table.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 16
#define DELETED  ((const char *)1)

typedef struct {
    const char *key;
    int value;
} Entry;

typedef struct {
    Entry *entries;
    size_t capacity;
    size_t count;
} HashTable;

static unsigned hash_fnv1a(const char *key) {
    unsigned h = 0x811c9dc5;
    while (*key) {
        h ^= (unsigned char)*key++;
        h *= 0x01000193;
    }
    return h;
}

HashTable *ht_create(size_t cap) {
    HashTable *ht = calloc(1, sizeof(HashTable));
    ht->capacity = cap;
    ht->entries = calloc(cap, sizeof(Entry));
    return ht;
}

void ht_destroy(HashTable *ht) {
    if (!ht) return;
    for (size_t i = 0; i < ht->capacity; i++) {
        if (ht->entries[i].key && ht->entries[i].key != DELETED) {
            free((void *)ht->entries[i].key);
        }
    }
    free(ht->entries);
    free(ht);
}

int ht_insert(HashTable *ht, const char *key, int value) {
    if (ht->count >= ht->capacity * 3 / 4) return -1;  /* 扩容阈值 */
    unsigned idx = hash_fnv1a(key) % ht->capacity;
    while (ht->entries[idx].key && ht->entries[idx].key != DELETED) {
        if (strcmp(ht->entries[idx].key, key) == 0) {
            ht->entries[idx].value = value;
            return 0;
        }
        idx = (idx + 1) % ht->capacity;
    }
    ht->entries[idx].key = strdup(key);
    ht->entries[idx].value = value;
    ht->count++;
    return 0;
}

int ht_get(HashTable *ht, const char *key, int *out) {
    unsigned idx = hash_fnv1a(key) % ht->capacity;
    while (ht->entries[idx].key) {
        if (ht->entries[idx].key != DELETED &&
            strcmp(ht->entries[idx].key, key) == 0) {
            *out = ht->entries[idx].value;
            return 0;
        }
        idx = (idx + 1) % ht->capacity;
    }
    return -1;
}

int ht_delete(HashTable *ht, const char *key) {
    unsigned idx = hash_fnv1a(key) % ht->capacity;
    while (ht->entries[idx].key) {
        if (ht->entries[idx].key != DELETED &&
            strcmp(ht->entries[idx].key, key) == 0) {
            free((void *)ht->entries[idx].key);
            ht->entries[idx].key = DELETED;
            ht->count--;
            return 0;
        }
        idx = (idx + 1) % ht->capacity;
    }
    return -1;
}

int main(void) {
    HashTable *ht = ht_create(32);
    ht_insert(ht, "foo", 42);
    ht_insert(ht, "bar", 99);
    ht_insert(ht, "baz", 7);
    
    int v;
    if (ht_get(ht, "bar", &v) == 0) printf("bar = %d\n", v);
    if (ht_get(ht, "foo", &v) == 0) printf("foo = %d\n", v);
    
    ht_delete(ht, "bar");
    printf("bar after delete: %d\n", ht_get(ht, "bar", &v));
    
    ht_destroy(ht);
    return 0;
}
