/*
 * Auto-generated from: 12_Practice_Exercises\03_Data_Structure_Projects.md
 * Line: 93
 * Language: c
 * Block ID: 2b4b6418
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct HashTable HashTable;
typedef struct {
    const void *key;
    void *value;
    uint32_t hash;
    bool occupied;
    bool deleted;
} Entry;

typedef struct {
    uint32_t (*hash_func)(const void *);
    bool (*key_equal)(const void *, const void *);
} HashTableOps;

HashTable* ht_create(size_t initial_capacity, const HashTableOps *ops);
void ht_destroy(HashTable *ht);

bool ht_insert(HashTable *ht, const void *key, void *value);
void* ht_get(const HashTable *ht, const void *key);
bool ht_remove(HashTable *ht, const void *key);
bool ht_contains(const HashTable *ht, const void *key);
size_t ht_size(const HashTable *ht);
