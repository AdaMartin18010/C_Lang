/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\02_Leader_Election.md
 * Line: 263
 * Language: c
 * Block ID: 060494c5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 获取随机选举超时 */
uint32_t get_random_election_timeout(RaftNode *node) {
    /* 使用当前时间作为随机种子的一部分 */
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    /* 简单的伪随机数生成 */
    uint32_t range = node->election_timeout_max - node->election_timeout_min;
    uint32_t random_offset = (ts.tv_nsec / 1000 + node->node_id * 17) % range;

    return node->election_timeout_min + random_offset;
}

/* 选举超时检查线程 */
void* election_timeout_thread(void *arg) {
    RaftNode *node = (RaftNode *)arg;

    while (node->running) {
        uint32_t timeout = get_random_election_timeout(node);

        struct timespec deadline;
        clock_gettime(CLOCK_MONOTONIC, &deadline);
        deadline.tv_sec += timeout / 1000;
        deadline.tv_nsec += (timeout % 1000) * 1000000;

        pthread_mutex_lock(&node->lock);

        /* 等待超时或被唤醒 */
        while (node->running && node->role != ROLE_LEADER) {
            int rc = pthread_cond_timedwait(&node->cond, &node->lock, &deadline);

            if (rc == ETIMEDOUT) {
                /* 选举超时触发 */
                uint64_t now = get_monotonic_ms();

                if (now - node->last_heartbeat >= timeout) {
                    pthread_mutex_unlock(&node->lock);
                    become_candidate(node);
                    goto next_election;
                }
            }
        }

        pthread_mutex_unlock(&node->lock);

next_election:
        if (node->role == ROLE_LEADER) {
            /* Leader不需要选举超时 */
            pthread_mutex_lock(&node->lock);
            while (node->running && node->role == ROLE_LEADER) {
                pthread_cond_wait(&node->cond, &node->lock);
            }
            pthread_mutex_unlock(&node->lock);
        }
    }

    return NULL;
}

/* 重置选举超时 */
void reset_election_timeout(RaftNode *node) {
    node->last_heartbeat = get_monotonic_ms();
    pthread_cond_broadcast(&node->cond);
}
