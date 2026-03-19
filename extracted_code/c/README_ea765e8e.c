/*
 * Auto-generated from: 07_Modern_Toolchain\10_Distributed_Systems\README.md
 * Line: 435
 * Language: c
 * Block ID: ea765e8e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 基于 Gossip 协议的最终一致性
typedef struct {
    char key[256];
    void* value;
    size_t len;
    uint64_t version;     // 版本号 (通常是时间戳或逻辑时钟)
    uint64_t timestamp;   // 物理时间戳
} gossip_entry_t;

typedef struct {
    gossip_entry_t** entries;
    size_t count;
    size_t capacity;
    pthread_rwlock_t lock;
} gossip_store_t;

// 反熵 Gossip - 定期与其他节点同步
void* anti_entropy_gossip(void* arg) {
    gossip_store_t* store = (gossip_store_t*)arg;

    while (running) {
        sleep(GOSSIP_INTERVAL);

        // 随机选择对等节点
        int peer = select_random_peer();

        // 发送本节点的摘要 (checksum 或版本范围)
        digest_t* local_digest = compute_digest(store);
        send_digest(peer, local_digest);

        // 接收对方的差异请求
        diff_request_t* request = receive_diff_request(peer);

        // 发送对方缺失的条目
        for (size_t i = 0; i < request->missing_count; i++) {
            gossip_entry_t* entry = find_entry(store, request->missing_keys[i]);
            if (entry) {
                send_entry(peer, entry);
            }
        }

        // 接收对方发送的差异条目
        while (has_more_entries(peer)) {
            gossip_entry_t* entry = receive_entry(peer);
            merge_entry(store, entry);
        }
    }
    return NULL;
}

// 合并条目 - 处理冲突
void merge_entry(gossip_store_t* store, gossip_entry_t* remote) {
    pthread_rwlock_wrlock(&store->lock);

    gossip_entry_t* local = find_entry(store, remote->key);

    if (!local) {
        // 本地不存在，直接添加
        add_entry(store, remote);
    } else if (remote->version > local->version) {
        // 远程版本更新，替换
        replace_entry(store, local, remote);
    } else if (remote->version == local->version) {
        // 版本相同但值不同 - 冲突！
        // 使用最后写入胜利 (LWW) 或其他策略
        if (remote->timestamp > local->timestamp) {
            replace_entry(store, local, remote);
        }
    }
    // 否则本地版本更新，忽略远程

    pthread_rwlock_unlock(&store->lock);
}
