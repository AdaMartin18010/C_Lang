/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\02_Leader_Election.md
 * Line: 483
 * Language: c
 * Block ID: 88b8426e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 心跳发送线程 */
void* heartbeat_thread(void *arg) {
    RaftNode *node = (RaftNode *)arg;

    while (node->running) {
        pthread_mutex_lock(&node->lock);

        if (node->role != ROLE_LEADER) {
            pthread_mutex_unlock(&node->lock);
            usleep(10000);  /* 10ms */
            continue;
        }

        pthread_mutex_unlock(&node->lock);

        /* 发送心跳 */
        send_heartbeat(node);

        /* 等待下一个心跳间隔 */
        usleep(node->heartbeat_interval * 1000);
    }

    return NULL;
}

/* 发送心跳/AppendEntries */
void send_heartbeat(RaftNode *node) {
    pthread_mutex_lock(&node->lock);

    if (node->role != ROLE_LEADER) {
        pthread_mutex_unlock(&node->lock);
        return;
    }

    uint64_t term = node->current_term;
    uint32_t leader_commit = node->commit_index;

    pthread_mutex_unlock(&node->lock);

    for (uint32_t i = 0; i < node->peer_count; i++) {
        pthread_mutex_lock(&node->lock);

        uint32_t next_idx = node->next_index[i];
        uint32_t prev_log_index = next_idx - 1;
        uint64_t prev_log_term = 0;

        if (prev_log_index > 0 && prev_log_index <= node->log_count) {
            prev_log_term = node->log[prev_log_index - 1].term;
        }

        /* 准备entries */
        uint32_t entry_count = 0;
        LogEntry entries[10];

        if (next_idx <= node->log_count) {
            uint32_t max_entries = 10;
            entry_count = node->log_count - next_idx + 1;
            if (entry_count > max_entries) entry_count = max_entries;

            for (uint32_t j = 0; j < entry_count; j++) {
                entries[j] = node->log[next_idx - 1 + j];
            }
        }

        pthread_mutex_unlock(&node->lock);

        /* 构建消息 */
        RaftMessage msg = {
            .type = entry_count > 0 ? MSG_APPEND_ENTRIES : MSG_HEARTBEAT,
            .from = node->node_id,
            .to = node->peers[i],
            .term = term,
            .payload.append = {
                .prev_log_index = prev_log_index,
                .prev_log_term = prev_log_term,
                .leader_commit = leader_commit,
                .entry_count = entry_count,
            }
        };
        memcpy(msg.payload.append.entries, entries,
               entry_count * sizeof(LogEntry));

        send_message_async(node, &msg);
    }
}

/* 处理心跳/AppendEntries */
void handle_append_entries(RaftNode *node, RaftMessage *msg) {
    pthread_mutex_lock(&node->lock);

    /* 任期检查 */
    if (msg->term < node->current_term) {
        RaftMessage response = {
            .type = MSG_APPEND_RESPONSE,
            .from = node->node_id,
            .to = msg->from,
            .term = node->current_term,
            .payload.append_rsp.success = false,
        };
        pthread_mutex_unlock(&node->lock);
        send_message(node, &response);
        return;
    }

    if (msg->term > node->current_term) {
        become_follower(node, msg->term);
    }

    /* 确认来自有效leader */
    node->last_heartbeat = get_monotonic_ms();
    reset_election_timeout(node);

    /* 日志一致性检查 */
    bool success = false;
    uint32_t match_index = 0;

    if (msg->payload.append.prev_log_index == 0) {
        success = true;  /* 第一个entry */
    } else if (msg->payload.append.prev_log_index <= node->log_count) {
        if (node->log[msg->payload.append.prev_log_index - 1].term ==
            msg->payload.append.prev_log_term) {
            success = true;
        }
    }

    if (success) {
        /* 追加新entries */
        uint32_t new_index = msg->payload.append.prev_log_index;

        for (uint32_t i = 0; i < msg->payload.append.entry_count; i++) {
            new_index++;
            LogEntry *entry = &msg->payload.append.entries[i];

            if (new_index <= node->log_count) {
                /* 冲突检测 */
                if (node->log[new_index - 1].term != entry->term) {
                    /* 截断日志 */
                    node->log_count = new_index - 1;
                } else {
                    continue;  /* 已存在相同entry */
                }
            }

            /* 追加entry */
            if (node->log_count >= node->log_capacity) {
                /* 扩容 */
                node->log_capacity *= 2;
                node->log = realloc(node->log,
                                    node->log_capacity * sizeof(LogEntry));
            }

            node->log[node->log_count++] = *entry;
        }

        match_index = new_index;

        /* 更新commit index */
        if (msg->payload.append.leader_commit > node->commit_index) {
            node->commit_index = msg->payload.append.leader_commit;
            if (node->commit_index > node->log_count) {
                node->commit_index = node->log_count;
            }
        }
    }

    pthread_mutex_unlock(&node->lock);

    /* 发送响应 */
    RaftMessage response = {
        .type = MSG_APPEND_RESPONSE,
        .from = node->node_id,
        .to = msg->from,
        .term = node->current_term,
        .payload.append_rsp = {
            .success = success,
            .match_index = match_index,
        }
    };
    send_message(node, &response);
}
