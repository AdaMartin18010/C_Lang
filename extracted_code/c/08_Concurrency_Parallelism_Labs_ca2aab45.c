/*
 * Auto-generated from: 12_Practice_Exercises\08_Concurrency_Parallelism_Labs.md
 * Line: 145
 * Language: c
 * Block ID: ca2aab45
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    _Atomic(Node*) *buckets;
    size_t bucket_count;
} LockFreeHashTable;

bool lfht_insert(LockFreeHashTable *ht, uint64_t key, void *value);
bool lfht_lookup(LockFreeHashTable *ht, uint64_t key, void **value);
bool lfht_remove(LockFreeHashTable *ht, uint64_t key);
