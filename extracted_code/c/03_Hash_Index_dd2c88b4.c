/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\03_Hash_Index.md
 * Line: 133
 * Language: c
 * Block ID: dd2c88b4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用开放寻址法的线性探测
typedef struct {
    char* key;
    void* value;
    uint8_t status;  // 0=空, 1=占用, 2=删除
} open_entry_t;

typedef struct {
    open_entry_t* entries;
    size_t size;
    size_t count;
} open_hash_table_t;

void* open_hash_get(open_hash_table_t* ht, const char* key) {
    uint64_t hash = fnv1a_hash(key);
    size_t idx = hash % ht->size;

    // 线性探测
    for (size_t i = 0; i < ht->size; i++) {
        size_t probe = (idx + i) % ht->size;
        open_entry_t* entry = &ht->entries[probe];

        if (entry->status == 0) {
            return NULL;  // 找到空槽，键不存在
        }
        if (entry->status == 1 && strcmp(entry->key, key) == 0) {
            return entry->value;
        }
    }
    return NULL;
}
