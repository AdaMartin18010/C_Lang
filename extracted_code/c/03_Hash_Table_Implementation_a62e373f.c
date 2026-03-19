/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\03_Hash_Table_Implementation.md
 * Line: 1296
 * Language: c
 * Block ID: a62e373f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 简单线程安全哈希表 (基于读写锁)
 */

#include <pthread.h>

typedef struct {
    ChainHashTable table;
    pthread_rwlock_t lock;
} ThreadSafeHashTable;

void ts_hash_init(ThreadSafeHashTable* ts_table) {
    chain_hash_init(&ts_table->table);
    pthread_rwlock_init(&ts_table->lock, nullptr);
}

void ts_hash_destroy(ThreadSafeHashTable* ts_table) {
    chain_hash_destroy(&ts_table->table);
    pthread_rwlock_destroy(&ts_table->lock);
}

bool ts_hash_insert(ThreadSafeHashTable* ts_table, int key, int value) {
    pthread_rwlock_wrlock(&ts_table->lock);
    bool result = chain_hash_insert(&ts_table->table, key, value);
    pthread_rwlock_unlock(&ts_table->lock);
    return result;
}

bool ts_hash_find(ThreadSafeHashTable* ts_table, int key, int* value) {
    pthread_rwlock_rdlock(&ts_table->lock);
    bool result = chain_hash_find(&ts_table->table, key, value);
    pthread_rwlock_unlock(&ts_table->lock);
    return result;
}

// 无锁哈希表 (使用原子操作)
// 概念性代码，实际实现较复杂
typedef struct {
    _Atomic(int) key;
    _Atomic(int) value;
    _Atomic(int) state;
} AtomicEntry;
