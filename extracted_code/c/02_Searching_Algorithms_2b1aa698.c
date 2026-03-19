/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\02_Searching_Algorithms.md
 * Line: 1504
 * Language: c
 * Block ID: 2b1aa698
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 无锁读的二分搜索（适用于读多写少场景）
 */
#include <stdatomic.h>

typedef struct {
    atomic_size_t version;
    int data[];
} VersionedArray;

int lock_free_search(VersionedArray *varr, size_t n, int target) {
    size_t version_before, version_after;
    int result = -1;

    do {
        version_before = atomic_load(&varr->version);

        // 复制数据（或使用双重检查）
        int *snapshot = malloc(n * sizeof(int));
        memcpy(snapshot, varr->data, n * sizeof(int));

        // 执行搜索
        result = binary_search_basic(snapshot, n, target);
        free(snapshot);

        version_after = atomic_load(&varr->version);
    } while (version_before != version_after);

    return result;
}
