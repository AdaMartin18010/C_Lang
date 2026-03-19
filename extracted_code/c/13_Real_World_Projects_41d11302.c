/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 2869
 * Language: c
 * Block ID: 41d11302
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef KVSTORE_H
#define KVSTORE_H

#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include "resp.h"

#define MAX_KEY_SIZE 512
#define MAX_DB_COUNT 16

/* Data types */
typedef enum {
    TYPE_STRING = 0,
    TYPE_LIST = 1,
    TYPE_HASH = 2,
    TYPE_SET = 3,
    TYPE_ZSET = 4
} value_type_t;

/* Value structure with expiration */
typedef struct kv_value {
    value_type_t type;
    void *data;
    size_t size;
    time_t expires_at;  /* 0 = no expiration */
} kv_value_t;

/* Hash table entry */
typedef struct kv_entry {
    char key[MAX_KEY_SIZE];
    kv_value_t value;
    struct kv_entry *next;  /* Separate chaining */
} kv_entry_t;

/* Database */
typedef struct kv_db {
    kv_entry_t **table;
    size_t capacity;
    size_t count;
    uint64_t expires_count;
    pthread_rwlock_t lock;
} kv_db_t;

/* Store */
typedef struct kv_store {
    kv_db_t databases[MAX_DB_COUNT];
    int active_db;
    uint64_t total_commands;
    uint64_t total_keys;
    time_t started_at;
} kv_store_t;

/* Lifecycle */
kv_store_t* kvstore_create(void);
void kvstore_destroy(kv_store_t *store);

/* Key operations */
int kvstore_set(kv_store_t *store, int db, const char *key,
                const kv_value_t *value);
int kvstore_get(kv_store_t *store, int db, const char *key,
                kv_value_t *out);
int kvstore_del(kv_store_t *store, int db, const char *key);
bool kvstore_exists(kv_store_t *store, int db, const char *key);
int kvstore_expire(kv_store_t *store, int db, const char *key, int seconds);
int kvstore_ttl(kv_store_t *store, int db, const char *key);

/* Database operations */
int kvstore_select(kv_store_t *store, int db);
int kvstore_flushdb(kv_store_t *store, int db);
int kvstore_flushall(kv_store_t *store);
size_t kvstore_dbsize(kv_store_t *store, int db);

/* Expiration handling */
void kvstore_expire_cycle(kv_store_t *store);
int kvstore_random_expire(kv_store_t *store, int db, int sample_size);

/* Memory stats */
void kvstore_memory_stats(const kv_store_t *store, size_t *used, size_t *peak);

#endif /* KVSTORE_H */
