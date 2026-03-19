/*
 * Auto-generated from: 07_Modern_Toolchain\10_Distributed_Systems\01_Consensus_Algorithms.md
 * Line: 179
 * Language: c
 * Block ID: adace050
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 反熵传播 */

typedef struct {
    char key[64];
    char value[256];
    int version;        /* 向量时钟 */
} gossip_data_t;

typedef struct {
    gossip_data_t* data;
    int count;
} gossip_digest_t;

/* 定期与随机节点交换状态 */
void gossip_round(void) {
    int peer = select_random_peer();

    /* 发送自己的摘要 */
    gossip_digest_t my_digest = get_digest();
    send_digest(peer, &my_digest);

    /* 接收对方的摘要 */
    gossip_digest_t peer_digest;
    recv_digest(peer, &peer_digest);

    /* 比较版本，请求缺失或更新的数据 */
    for (int i = 0; i < peer_digest.count; i++) {
        gossip_data_t* d = &peer_digest.data[i];
        gossip_data_t* local = find_local(d->key);

        if (!local || d->version > local->version) {
            request_data(peer, d->key);
        }
    }
}

/* 广播新数据 */
void gossip_broadcast(gossip_data_t* data) {
    data->version = increment_vector_clock();

    /* 发送给随机子集 */
    for (int i = 0; i < FANOUT; i++) {
        int peer = select_random_peer();
        send_data(peer, data);
    }
}
