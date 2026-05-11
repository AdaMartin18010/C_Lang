/*
 * 内存 KV 存储引擎实现
 * 哈希表 + 开放寻址 + 线性探测
 * 编译: gcc -Wall -Wextra -O2 -std=c11 -c kv_store.c
 */
#include "kv_store.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOMBSTONE ((char *)1)

typedef struct {
    char *key;
    char *value;
} Entry;

struct KVStore {
    Entry *entries;
    size_t capacity;
    size_t count;
    size_t tombstones;
};

static uint64_t hash_fnv1a(const char *key) {
    uint64_t h = 0xcbf29ce484222325ULL;
    while (*key) {
        h ^= (unsigned char)*key++;
        h *= 0x100000001b3ULL;
    }
    return h;
}

static Entry *find_entry(Entry *entries, size_t cap, const char *key) {
    uint64_t idx = hash_fnv1a(key) % cap;
    Entry *tomb = NULL;
    while (1) {
        Entry *e = &entries[idx];
        if (!e->key) {
            return tomb ? tomb : e;
        } else if (e->key == TOMBSTONE) {
            if (!tomb) tomb = e;
        } else if (strcmp(e->key, key) == 0) {
            return e;
        }
        idx = (idx + 1) % cap;
    }
}

static int grow(KVStore *store) {
    size_t old_cap = store->capacity;
    size_t new_cap = old_cap < 8 ? 8 : old_cap * 2;
    Entry *new_entries = calloc(new_cap, sizeof(Entry));
    if (!new_entries) return -1;
    
    for (size_t i = 0; i < old_cap; i++) {
        Entry *e = &store->entries[i];
        if (e->key && e->key != TOMBSTONE) {
            Entry *dest = find_entry(new_entries, new_cap, e->key);
            *dest = *e;
        }
    }
    free(store->entries);
    store->entries = new_entries;
    store->capacity = new_cap;
    store->tombstones = 0;
    return 0;
}

KVStore *kv_create(size_t capacity) {
    KVStore *store = calloc(1, sizeof(KVStore));
    if (!store) return NULL;
    store->capacity = capacity > 0 ? capacity : 16;
    store->entries = calloc(store->capacity, sizeof(Entry));
    if (!store->entries) {
        free(store);
        return NULL;
    }
    return store;
}

void kv_destroy(KVStore *store) {
    if (!store) return;
    for (size_t i = 0; i < store->capacity; i++) {
        if (store->entries[i].key && store->entries[i].key != TOMBSTONE) {
            free(store->entries[i].key);
            free(store->entries[i].value);
        }
    }
    free(store->entries);
    free(store);
}

void kv_clear(KVStore *store) {
    if (!store) return;
    for (size_t i = 0; i < store->capacity; i++) {
        if (store->entries[i].key && store->entries[i].key != TOMBSTONE) {
            free(store->entries[i].key);
            free(store->entries[i].value);
        }
        store->entries[i].key = NULL;
        store->entries[i].value = NULL;
    }
    store->count = 0;
    store->tombstones = 0;
}

int kv_set(KVStore *store, const char *key, const char *value) {
    if (!store || !key || !value) return -1;
    if (store->count + store->tombstones >= store->capacity * 3 / 4) {
        if (grow(store) < 0) return -1;
    }
    Entry *e = find_entry(store->entries, store->capacity, key);
    if (e->key && e->key != TOMBSTONE) {
        free(e->value);
        e->value = strdup(value);
        return 0;
    }
    e->key = strdup(key);
    e->value = strdup(value);
    store->count++;
    return 0;
}

const char *kv_get(KVStore *store, const char *key) {
    if (!store || !key) return NULL;
    Entry *e = find_entry(store->entries, store->capacity, key);
    return (e->key && e->key != TOMBSTONE) ? e->value : NULL;
}

int kv_delete(KVStore *store, const char *key) {
    if (!store || !key) return -1;
    Entry *e = find_entry(store->entries, store->capacity, key);
    if (!e->key || e->key == TOMBSTONE) return -1;
    free(e->key);
    free(e->value);
    e->key = TOMBSTONE;
    e->value = NULL;
    store->count--;
    store->tombstones++;
    return 0;
}

bool kv_exists(KVStore *store, const char *key) {
    return kv_get(store, key) != NULL;
}

void kv_foreach(KVStore *store, KVIterFn fn, void *ctx) {
    if (!store || !fn) return;
    for (size_t i = 0; i < store->capacity; i++) {
        Entry *e = &store->entries[i];
        if (e->key && e->key != TOMBSTONE) {
            fn(e->key, e->value, ctx);
        }
    }
}

size_t kv_count(const KVStore *store) {
    return store ? store->count : 0;
}

size_t kv_capacity(const KVStore *store) {
    return store ? store->capacity : 0;
}

/* 简单文本格式持久化: KEY\tVALUE\n */
int kv_save(KVStore *store, const char *path) {
    if (!store || !path) return -1;
    FILE *fp = fopen(path, "w");
    if (!fp) return -1;
    for (size_t i = 0; i < store->capacity; i++) {
        Entry *e = &store->entries[i];
        if (e->key && e->key != TOMBSTONE) {
            fprintf(fp, "%s\t%s\n", e->key, e->value);
        }
    }
    fclose(fp);
    return 0;
}

int kv_load(KVStore *store, const char *path) {
    if (!store || !path) return -1;
    FILE *fp = fopen(path, "r");
    if (!fp) return -1;
    char line[4096];
    while (fgets(line, sizeof(line), fp)) {
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') line[len-1] = '\0';
        char *tab = strchr(line, '\t');
        if (tab) {
            *tab = '\0';
            kv_set(store, line, tab + 1);
        }
    }
    fclose(fp);
    return 0;
}
