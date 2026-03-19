/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\README.md
 * Line: 333
 * Language: c
 * Block ID: 90d45418
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 领导者处理客户端请求 */
bool raft_propose(raft_node_t *node, const uint8_t *data, size_t len) {
    if (node->state != RAFT_STATE_LEADER) {
        return false;  /* 非领导者拒绝 */
    }

    /* 追加到本地日志 */
    raft_log_entry_t *entry = malloc(sizeof(raft_log_entry_t) + len);
    entry->term = node->current_term;
    entry->index = raft_get_last_log_index(node) + 1;
    memcpy(entry->data, data, len);

    /* 追加到日志 */
    node->log = realloc(node->log, (node->log_count + 1) * sizeof(void*));
    node->log[node->log_count++] = entry;
    raft_persist_log(node, entry);

    /* 立即发送AppendEntries给所有跟随者 */
    for (int i = 0; i < node->node_count; i++) {
        if (node->peer_ids[i] == node->node_id) continue;
        raft_send_append_entries(node, node->peer_ids[i]);
    }

    return true;
}

/* 发送AppendEntries RPC */
void raft_send_append_entries(raft_node_t *node, int peer_id) {
    int peer_idx = get_peer_index(node, peer_id);
    uint64_t next_idx = node->next_index[peer_idx];

    append_entries_req_t req = {
        .term = node->current_term,
        .leader_id = node->node_id,
        .prev_log_index = next_idx - 1,
        .prev_log_term = (next_idx > 1) ?
            node->log[next_idx - 2]->term : 0,
        .leader_commit = node->commit_index,
        .entries = NULL,
        .entry_count = 0
    };

    /* 准备要发送的条目（从next_index开始） */
    uint64_t entries_to_send = node->log_count - (next_idx - 1);
    if (entries_to_send > MAX_ENTRIES_PER_RPC)
        entries_to_send = MAX_ENTRIES_PER_RPC;

    if (entries_to_send > 0) {
        req.entries = &node->log[next_idx - 1];
        req.entry_count = entries_to_send;
    }

    raft_send_rpc(node, peer_id, RAFT_MSG_APPEND_ENTRIES,
                  &req, sizeof(req) - sizeof(void*) +
                  req.entry_count * sizeof(raft_log_entry_t*));
}

/* 处理AppendEntries RPC（跟随者端） */
void raft_handle_append_entries(raft_node_t *node, int from,
                                append_entries_req_t *req,
                                append_entries_resp_t *resp) {
    resp->term = node->current_term;
    resp->success = false;
    resp->match_index = 0;
    resp->conflict_term = 0;
    resp->conflict_index = 0;

    /* 1. 如果领导者任期更低，拒绝 */
    if (req->term < node->current_term) {
        return;
    }

    /* 2. 收到有效领导者心跳，重置超时 */
    node->last_heartbeat = get_monotonic_ms();

    /* 3. 如果收到更高任期，转换为跟随者 */
    if (req->term > node->current_term) {
        raft_become_follower(node, req->term);
    }

    /* 4. 日志一致性检查 */
    if (req->prev_log_index > 0) {
        if (req->prev_log_index > raft_get_last_log_index(node)) {
            /* 日志太短，告知领导者正确的索引 */
            resp->conflict_index = raft_get_last_log_index(node) + 1;
            return;
        }

        if (node->log[req->prev_log_index - 1]->term != req->prev_log_term) {
            /* 任期不匹配，查找冲突任期的第一个索引 */
            resp->conflict_term = node->log[req->prev_log_index - 1]->term;
            uint64_t idx = req->prev_log_index;
            while (idx > 1 && node->log[idx - 2]->term == resp->conflict_term) {
                idx--;
            }
            resp->conflict_index = idx;
            return;
        }
    }

    /* 5. 日志匹配，追加新条目 */
    resp->success = true;

    uint64_t insert_idx = req->prev_log_index + 1;
    for (uint64_t i = 0; i < req->entry_count; i++) {
        raft_log_entry_t *new_entry = req->entries[i];

        if (insert_idx <= raft_get_last_log_index(node)) {
            /* 已存在条目，检查任期 */
            if (node->log[insert_idx - 1]->term != new_entry->term) {
                /* 删除冲突及之后的所有条目 */
                while (node->log_count >= insert_idx) {
                    free(node->log[--node->log_count]);
                }
                node->log = realloc(node->log,
                                    insert_idx * sizeof(void*));
            } else {
                /* 条目已存在且匹配，跳过 */
                insert_idx++;
                continue;
            }
        }

        /* 追加新条目 */
        node->log = realloc(node->log, (node->log_count + 1) * sizeof(void*));
        node->log[node->log_count] = malloc(sizeof(raft_log_entry_t) +
                                            strlen((char*)new_entry->data));
        memcpy(node->log[node->log_count], new_entry,
               sizeof(raft_log_entry_t) + strlen((char*)new_entry->data));
        node->log_count++;
        raft_persist_log(node, node->log[node->log_count - 1]);
        insert_idx++;
    }

    /* 6. 更新提交索引 */
    if (req->leader_commit > node->commit_index) {
        node->commit_index = min(req->leader_commit,
                                 raft_get_last_log_index(node));
        raft_apply_committed(node);
    }

    resp->match_index = req->prev_log_index + req->entry_count;
}

/* 领导者处理AppendEntries响应并推进commit_index */
void raft_handle_append_entries_response(raft_node_t *node, int from,
                                         append_entries_resp_t *resp) {
    if (resp->term > node->current_term) {
        raft_become_follower(node, resp->term);
        return;
    }

    if (node->state != RAFT_STATE_LEADER || resp->term != node->current_term)
        return;

    int peer_idx = get_peer_index(node, from);

    if (resp->success) {
        /* 成功复制，更新匹配索引 */
        node->match_index[peer_idx] = resp->match_index;
        node->next_index[peer_idx] = resp->match_index + 1;

        /* 检查是否可以提交 */
        raft_advance_commit_index(node);
    } else {
        /* 复制失败，回退next_index */
        if (resp->conflict_term > 0) {
            /* 优化回退：找到冲突任期的最后一条 */
            uint64_t idx = raft_get_last_log_index(node);
            while (idx > 0 && node->log[idx - 1]->term > resp->conflict_term) {
                idx--;
            }
            if (idx > 0 && node->log[idx - 1]->term == resp->conflict_term) {
                node->next_index[peer_idx] = idx;
            } else {
                node->next_index[peer_idx] = resp->conflict_index;
            }
        } else {
            node->next_index[peer_idx] = resp->conflict_index;
        }

        /* 立即重试 */
        raft_send_append_entries(node, from);
    }
}

/* 推进提交索引 - 仅在当前任期条目上提交 */
void raft_advance_commit_index(raft_node_t *node) {
    for (uint64_t N = node->commit_index + 1;
         N <= raft_get_last_log_index(node); N++) {

        /* 只提交当前任期的条目（Raft安全约束） */
        if (node->log[N - 1]->term != node->current_term)
            continue;

        /* 统计在N或更高索引上匹配的节点数 */
        int match_count = 1;  /* 领导者自己 */
        for (int i = 0; i < node->node_count; i++) {
            if (node->peer_ids[i] == node->node_id) continue;
            int peer_idx = get_peer_index(node, node->peer_ids[i]);
            if (node->match_index[peer_idx] >= N)
                match_count++;
        }

        /* 如果超过半数匹配，可以提交 */
        if (match_count > node->node_count / 2) {
            node->commit_index = N;
        } else {
            break;  /* 无法再推进 */
        }
    }

    raft_apply_committed(node);
}

/* 应用已提交的日志到状态机 */
void raft_apply_committed(raft_node_t *node) {
    while (node->last_applied < node->commit_index) {
        node->last_applied++;
        raft_log_entry_t *entry = node->log[node->last_applied - 1];
        node->apply(node->last_applied, entry->data);
    }
}
