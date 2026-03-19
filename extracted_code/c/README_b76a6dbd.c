/*
 * Auto-generated from: 07_Modern_Toolchain\10_Distributed_Systems\README.md
 * Line: 362
 * Language: c
 * Block ID: b76a6dbd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 向量时钟实现因果一致性
typedef struct {
    uint64_t* clocks;    // 每个节点的逻辑时钟
    size_t node_count;   // 节点数量
} vector_clock_t;

// 比较向量时钟
typedef enum {
    VC_LESS,      // vc1 < vc2
    VC_GREATER,   // vc1 > vc2
    VC_EQUAL,     // vc1 == vc2
    VC_CONCURRENT // vc1 || vc2 (并发)
} vc_compare_result_t;

vc_compare_result_t vector_clock_compare(const vector_clock_t* vc1,
                                         const vector_clock_t* vc2) {
    bool less = false, greater = false;

    for (size_t i = 0; i < vc1->node_count; i++) {
        if (vc1->clocks[i] < vc2->clocks[i]) {
            less = true;
        } else if (vc1->clocks[i] > vc2->clocks[i]) {
            greater = true;
        }
    }

    if (less && !greater) return VC_LESS;
    if (greater && !less) return VC_GREATER;
    if (!less && !greater) return VC_EQUAL;
    return VC_CONCURRENT;
}

// 带向量时钟的数据版本
typedef struct {
    void* data;
    size_t len;
    vector_clock_t* vc;  // 版本向量时钟
} versioned_data_t;

// 因果一致性写
void causal_write(causal_store_t* store, const char* key,
                  const void* value, size_t len) {
    // 递增本节点的逻辑时钟
    store->local_clock->clocks[store->node_id]++;

    versioned_data_t* new_version = malloc(sizeof(versioned_data_t));
    new_version->data = malloc(len);
    memcpy(new_version->data, value, len);
    new_version->len = len;

    // 复制当前向量时钟
    new_version->vc = vector_clock_copy(store->local_clock);

    // 存储新版本 (可能保留多个版本处理并发)
    store_put_versioned(store, key, new_version);

    // 异步复制到其他节点
    replicate_async(store, key, new_version);
}

// 因果一致性读
versioned_data_t* causal_read(causal_store_t* store, const char* key) {
    version_list_t* versions = store_get_versions(store, key);

    // 返回最新的因果依赖版本
    // 如果有并发版本，可能需要合并或返回所有版本
    return select_causal_latest(versions, store->local_clock);
}
