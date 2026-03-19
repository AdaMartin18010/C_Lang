/*
 * Auto-generated from: 07_Modern_Toolchain\10_Distributed_Systems\README.md
 * Line: 307
 * Language: c
 * Block ID: cb53d4d7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 基于 Paxos/Raft 的线性一致性存储
typedef struct {
    raft_node_t* raft;           // 底层共识层
    hash_table_t* data;          // 数据存储
    pthread_mutex_t apply_mutex; // 状态机互斥锁
} linear_store_t;

// 写操作 - 必须通过共识
int linear_store_write(linear_store_t* store, const char* key,
                       const void* value, size_t len) {
    // 构建写操作日志条目
    write_op_t op = {
        .type = OP_WRITE,
        .key = strdup(key),
        .value = malloc(len),
        .value_len = len
    };
    memcpy(op.value, value, len);

    // 提交到 Raft 集群
    int index = raft_append_entry(store->raft, &op, sizeof(op));
    if (index < 0) return -1;

    // 等待提交并应用到状态机
    return wait_for_apply(store->raft, index);
}

// 读操作 - 在领导者上执行确保线性一致性
int linear_store_read(linear_store_t* store, const char* key,
                      void** value, size_t* len) {
    // 确保本节点是领导者
    if (store->raft->state != STATE_LEADER) {
        return redirect_to_leader(store->raft, key);
    }

    // 提交一个空操作(no-op)来确保所有之前的写入已提交
    raft_append_entry(store->raft, NULL, 0);

    // 从状态机读取
    pthread_mutex_lock(&store->apply_mutex);
    entry_t* entry = hash_table_get(store->data, key);
    pthread_mutex_unlock(&store->apply_mutex);

    if (!entry) return -1;

    *value = malloc(entry->len);
    memcpy(*value, entry->data, entry->len);
    *len = entry->len;
    return 0;
}
