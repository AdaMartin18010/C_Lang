/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\03_Hash_Table_Implementation.md
 * Line: 867
 * Language: c
 * Block ID: fd19673a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 通用哈希表接口设计
 * 支持任意键值类型
 */

typedef unsigned int (*HashFunc)(const void* key, size_t len);
typedef bool (*KeyEqualFunc)(const void* key1, const void* key2);
typedef void (*FreeFunc)(void* ptr);

typedef struct {
    void* key;
    void* value;
    size_t key_len;
} GenericEntry;

typedef struct {
    GenericEntry* entries;
    size_t capacity;
    size_t size;

    HashFunc hash_fn;
    KeyEqualFunc equal_fn;
    FreeFunc key_free_fn;
    FreeFunc value_free_fn;
} GenericHashTable;

// 初始化
void generic_hash_init(GenericHashTable* table,
                       HashFunc hash_fn,
                       KeyEqualFunc equal_fn) {
    table->capacity = 16;
    table->entries = (GenericEntry*)calloc(table->capacity, sizeof(GenericEntry));
    table->size = 0;
    table->hash_fn = hash_fn ? hash_fn : default_hash;
    table->equal_fn = equal_fn ? equal_fn : default_equal;
    table->key_free_fn = nullptr;
    table->value_free_fn = nullptr;
}

// 字符串哈希表专用接口
unsigned int str_hash(const void* key, size_t len) {
    (void)len;
    return fnv1a_hash((const char*)key);
}

bool str_equal(const void* key1, const void* key2) {
    return strcmp((const char*)key1, (const char*)key2) == 0;
}
