/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\02_Intermediate_Learning_Path.md
 * Line: 272
 * Language: c
 * Block ID: befbd7e9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stddef.h>
#include <stdbool.h>

#define HT_INITIAL_CAPACITY 16
#define HT_LOAD_FACTOR 0.75

typedef struct HashEntry {
    char *key;
    void *value;
    struct HashEntry *next;  // 链地址法
} HashEntry;

typedef struct {
    HashEntry **buckets;
    size_t capacity;
    size_t size;
    size_t value_size;
    void (*free_value)(void *);
} HashTable;

// 核心API
HashTable* ht_create(size_t value_size, void (*free_value)(void *));
void ht_destroy(HashTable *ht);
int ht_insert(HashTable *ht, const char *key, const void *value);
bool ht_get(HashTable *ht, const char *key, void *out);
bool ht_remove(HashTable *ht, const char *key);
bool ht_contains(HashTable *ht, const char *key);

// 迭代支持
typedef struct {
    HashTable *ht;
    size_t bucket_idx;
    HashEntry *current;
} HTIterator;

HTIterator ht_iterator(HashTable *ht);
bool ht_next(HTIterator *it, char **key, void **value);

#endif
