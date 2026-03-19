/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\07_Performance_Optimization.md
 * Line: 124
 * Language: c
 * Block ID: e37705a3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Robin Hood Hashing - 缓存友好

typedef struct {
    uint64_t key;
    void *value;
    int32_t probe_len;  // 用于Robin Hood交换
} Entry;

typedef struct {
    Entry *entries;
    int size;
    int count;
} RobinHoodHash;

void* rh_get(RobinHoodHash *ht, uint64_t key) {
    int idx = hash(key) & (ht->size - 1);  // size必须是2的幂
    int probe = 0;

    while (1) {
        Entry *e = &ht->entries[idx];
        if (e->key == 0) return NULL;  // 空槽
        if (e->key == key) return e->value;

        // Robin Hood：如果当前元素探测距离更小，停止搜索
        if (e->probe_len < probe) return NULL;

        idx = (idx + 1) & (ht->size - 1);
        probe++;
    }
}

// 优化效果：
// - 连续内存访问（缓存友好）
// - 无链表指针跳转
// - Robin Hood平衡探测长度
