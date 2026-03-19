/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\02_Leader_Election.md
 * Line: 196
 * Language: c
 * Block ID: 422a2158
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 转换到Follower状态 */
void become_follower(RaftNode *node, uint64_t term) {
    pthread_mutex_lock(&node->lock);

    node->role = ROLE_FOLLOWER;
    node->current_term = term;
    node->voted_for = 0;  /* 重置投票 */

    pthread_mutex_unlock(&node->lock);

    printf("Node %d became FOLLOWER at term %lu\n", node->node_id, term);
}

/* 转换到Candidate状态 */
void become_candidate(RaftNode *node) {
    pthread_mutex_lock(&node->lock);

    node->role = ROLE_CANDIDATE;
    node->current_term++;
    node->voted_for = node->node_id;  /* 投给自己 */

    uint64_t term = node->current_term;

    pthread_mutex_unlock(&node->lock);

    printf("Node %d became CANDIDATE at term %lu\n", node->node_id, term);

    /* 立即开始选举 */
    start_election(node);
}

/* 转换到Leader状态 */
void become_leader(RaftNode *node) {
    pthread_mutex_lock(&node->lock);

    if (node->role != ROLE_CANDIDATE) {
        pthread_mutex_unlock(&node->lock);
        return;
    }

    node->role = ROLE_LEADER;

    /* 初始化leader状态 */
    uint32_t last_log_index = node->log_count > 0 ?
                              node->log[node->log_count - 1].index + 1 : 1;

    for (uint32_t i = 0; i < node->peer_count; i++) {
        node->next_index[i] = last_log_index;
        node->match_index[i] = 0;
    }

    pthread_mutex_unlock(&node->lock);

    printf("Node %d became LEADER at term %lu\n", node->node_id, node->current_term);

    /* 立即发送心跳 */
    send_heartbeat(node);
}
