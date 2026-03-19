/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\03_Hash_Table_Implementation.md
 * Line: 802
 * Language: c
 * Block ID: 5fd60a60
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 渐进式哈希 (Incremental Hashing)
 * 避免一次性扩容导致的延迟峰值
 */
typedef struct {
    HashEntry* old_entries;
    HashEntry* new_entries;
    size_t old_capacity;
    size_t new_capacity;
    size_t rehash_idx;    // 当前重哈希进度
    bool is_rehashing;
} ProgressiveHashTable;

// 查找 (需要检查两个表)
bool progressive_find(ProgressiveHashTable* table, int key, int* value) {
    // 先查新表
    if (table->new_entries != nullptr) {
        size_t idx = hash(key, table->new_capacity);
        if (table->new_entries[idx].key == key &&
            table->new_entries[idx].state == ENTRY_OCCUPIED) {
            *value = table->new_entries[idx].value;
            return true;
        }
    }

    // 再查旧表
    if (table->is_rehashing && table->old_entries != nullptr) {
        size_t idx = hash(key, table->old_capacity);
        // ... 类似查找
    }

    return false;
}

// 渐进重哈希 (每次操作迁移少量元素)
void progressive_rehash_step(ProgressiveHashTable* table, size_t n) {
    if (!table->is_rehashing) return;

    for (size_t i = 0; i < n && table->rehash_idx < table->old_capacity; i++) {
        // 迁移 rehash_idx 位置的元素
        HashEntry* entry = &table->old_entries[table->rehash_idx];
        if (entry->state == ENTRY_OCCUPIED) {
            // 插入新表
            size_t new_idx = hash(entry->key, table->new_capacity);
            table->new_entries[new_idx] = *entry;
        }
        table->rehash_idx++;
    }

    // 检查是否完成
    if (table->rehash_idx >= table->old_capacity) {
        free(table->old_entries);
        table->old_entries = nullptr;
        table->is_rehashing = false;
    }
}
