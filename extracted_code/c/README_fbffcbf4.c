/*
 * Auto-generated from: 07_Modern_Toolchain\10_Distributed_Systems\README.md
 * Line: 594
 * Language: c
 * Block ID: fbffcbf4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// AP 系统在分区时的行为：牺牲一致性保证可用性

typedef struct {
    bool partitioned;           // 是否处于分区状态
    hash_set_t* reachable_nodes;
    vector_clock_t* local_vc;   // 本地向量时钟
} ap_node_state_t;

// AP 系统写操作
int ap_system_write(ap_node_state_t* state, const char* key,
                    const void* value) {
    // 无论是否分区，都接受写入
    // 生成新的版本
    versioned_value_t* new_val = malloc(sizeof(versioned_value_t));
    new_val->data = malloc(value_len);
    memcpy(new_val->data, value, value_len);
    new_val->vc = vector_clock_increment(state->local_vc, my_node_id);

    // 本地存储
    local_store_put(key, new_val);

    // 异步复制给可达节点
    for (int node : state->reachable_nodes) {
        if (node != my_node_id) {
            async_replicate(node, key, new_val);
        }
    }

    // 立即返回成功 (即使其他分区看不到这个写入)
    return 0;
}

// AP 系统读操作
versioned_value_t* ap_system_read(ap_node_state_t* state, const char* key) {
    // 始终返回本地值，保证可用性
    versioned_value_t* local = local_store_get(key);

    // 异步尝试从其他节点获取更新版本
    for (int node : state->reachable_nodes) {
        async_fetch(node, key);
    }

    return local;
}

// 分区恢复后的冲突解决
void on_partition_heal(ap_node_state_t* state) {
    for (each key in local_store) {
        version_list_t* remote_versions = fetch_versions_from_peers(key);
        version_list_t* local_versions = local_store_get_all_versions(key);

        // 合并版本，解决冲突
        version_list_t* merged = merge_versions(local_versions,
                                                remote_versions);

        // 如果存在并发写入，需要根据业务规则解决
        if (has_concurrent_versions(merged)) {
            resolve_conflicts(key, merged);
        }
    }
}
