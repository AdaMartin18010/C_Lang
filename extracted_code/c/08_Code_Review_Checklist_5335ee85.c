/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 707
 * Language: c
 * Block ID: 5335ee85
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误示例：O(n²)的查找
// 在数组中查找重复项
int find_duplicate(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {  // O(n²)
            if (arr[i] == arr[j]) {
                return arr[i];
            }
        }
    }
    return -1;
}

// ✅ 优化后：O(n)的查找
#include <stdbool.h>

int find_duplicate_fast(int *arr, int n) {
    // 假设数值范围已知且在合理范围内
    bool seen[10000] = {false};

    for (int i = 0; i < n; i++) {  // O(n)
        if (arr[i] >= 0 && arr[i] < 10000) {
            if (seen[arr[i]]) {
                return arr[i];
            }
            seen[arr[i]] = true;
        }
    }
    return -1;
}

// ✅ 使用哈希表（更通用）
#include "uthash.h"  // 使用uthash库

struct hash_entry {
    int key;
    UT_hash_handle hh;
};

int find_duplicate_hash(int *arr, int n) {
    struct hash_entry *hash_table = NULL;

    for (int i = 0; i < n; i++) {
        struct hash_entry *entry;
        HASH_FIND_INT(hash_table, &arr[i], entry);
        if (entry != NULL) {
            // 清理
            struct hash_entry *current, *tmp;
            HASH_ITER(hh, hash_table, current, tmp) {
                HASH_DEL(hash_table, current);
                free(current);
            }
            return arr[i];
        }
        entry = malloc(sizeof(*entry));
        entry->key = arr[i];
        HASH_ADD_INT(hash_table, key, entry);
    }
    return -1;
}
