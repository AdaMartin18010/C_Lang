/*
 * 内存 KV 存储引擎 - 工业实战项目
 * 头文件
 */
#ifndef KV_STORE_H
#define KV_STORE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct KVStore KVStore;

/* 生命周期 */
KVStore *kv_create(size_t capacity);
void kv_destroy(KVStore *store);
void kv_clear(KVStore *store);

/* CRUD 操作 */
int kv_set(KVStore *store, const char *key, const char *value);
const char *kv_get(KVStore *store, const char *key);
int kv_delete(KVStore *store, const char *key);
bool kv_exists(KVStore *store, const char *key);

/* 遍历 */
typedef void (*KVIterFn)(const char *key, const char *value, void *ctx);
void kv_foreach(KVStore *store, KVIterFn fn, void *ctx);

/* 统计 */
size_t kv_count(const KVStore *store);
size_t kv_capacity(const KVStore *store);

/* 持久化（简单文本格式） */
int kv_save(KVStore *store, const char *path);
int kv_load(KVStore *store, const char *path);

#endif /* KV_STORE_H */
