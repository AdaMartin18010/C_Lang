/*
 * KV 存储测试与演示
 * 编译: gcc -Wall -Wextra -O2 -std=c11 -o test_kv kv_store.c test_kv.c
 */
#include "kv_store.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

void test_basic(void) {
    printf("=== Test: Basic CRUD ===\n");
    KVStore *store = kv_create(16);
    assert(store);
    
    kv_set(store, "name", "Alice");
    kv_set(store, "age", "30");
    kv_set(store, "city", "Beijing");
    
    assert(strcmp(kv_get(store, "name"), "Alice") == 0);
    assert(strcmp(kv_get(store, "age"), "30") == 0);
    assert(kv_exists(store, "city"));
    assert(!kv_exists(store, "nonexist"));
    
    kv_set(store, "age", "31");  /* update */
    assert(strcmp(kv_get(store, "age"), "31") == 0);
    
    kv_delete(store, "city");
    assert(!kv_exists(store, "city"));
    
    printf("Count: %zu, Capacity: %zu\n", kv_count(store), kv_capacity(store));
    assert(kv_count(store) == 2);
    
    kv_destroy(store);
    printf("PASS\n\n");
}

void test_persistence(void) {
    printf("=== Test: Persistence ===\n");
    KVStore *store = kv_create(16);
    kv_set(store, "foo", "bar");
    kv_set(store, "hello", "world");
    
    assert(kv_save(store, "kv_test.txt") == 0);
    kv_clear(store);
    assert(kv_count(store) == 0);
    
    assert(kv_load(store, "kv_test.txt") == 0);
    assert(kv_count(store) == 2);
    assert(strcmp(kv_get(store, "foo"), "bar") == 0);
    assert(strcmp(kv_get(store, "hello"), "world") == 0);
    
    kv_destroy(store);
    remove("kv_test.txt");
    printf("PASS\n\n");
}

static int iter_count = 0;
static void count_iter(const char *key, const char *value, void *ctx) {
    (void)key; (void)value;
    (*(int *)ctx)++;
}

void test_grow(void) {
    printf("=== Test: Auto Grow ===\n");
    KVStore *store = kv_create(4);
    
    for (int i = 0; i < 1000; i++) {
        char key[32], val[32];
        snprintf(key, sizeof(key), "key_%d", i);
        snprintf(val, sizeof(val), "val_%d", i);
        kv_set(store, key, val);
    }
    
    printf("Count: %zu, Capacity: %zu\n", kv_count(store), kv_capacity(store));
    assert(kv_count(store) == 1000);
    
    /* 验证随机访问 */
    assert(strcmp(kv_get(store, "key_500"), "val_500") == 0);
    assert(strcmp(kv_get(store, "key_999"), "val_999") == 0);
    
    /* 遍历 */
    iter_count = 0;
    kv_foreach(store, count_iter, &iter_count);
    assert(iter_count == 1000);
    
    kv_destroy(store);
    printf("PASS\n\n");
}

void benchmark(void) {
    printf("=== Benchmark: 100K ops ===\n");
    KVStore *store = kv_create(1024);
    
    /* Insert */
    for (int i = 0; i < 100000; i++) {
        char key[32], val[32];
        snprintf(key, sizeof(key), "k%d", i);
        snprintf(val, sizeof(val), "v%d", i);
        kv_set(store, key, val);
    }
    printf("Inserted 100K pairs, capacity: %zu\n", kv_capacity(store));
    
    /* Lookup */
    int hits = 0;
    for (int i = 0; i < 100000; i++) {
        char key[32];
        snprintf(key, sizeof(key), "k%d", i);
        if (kv_get(store, key)) hits++;
    }
    printf("Looked up 100K keys, hits: %d\n", hits);
    assert(hits == 100000);
    
    kv_destroy(store);
    printf("PASS\n\n");
}

int main(void) {
    test_basic();
    test_persistence();
    test_grow();
    benchmark();
    printf("All tests passed!\n");
    return 0;
}
