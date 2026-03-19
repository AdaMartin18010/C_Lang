/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\01_Raft_Core.md
 * Line: 260
 * Language: c
 * Block ID: 712931d8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 处理客户端请求（仅领导者）
int client_request(RaftNode *node, Command *cmd) {
    if (node->state != STATE_LEADER) {
        return -1;  // 不是领导者，拒绝
    }

    // 追加到本地日志
    LogEntry entry = {
        .term = node->current_term,
        .index = log_last_index(node) + 1,
        .cmd = *cmd
    };
    log_append(node, &entry);

    // 异步复制到跟随者
    replicate_log(node);

    return 0;
}

// 发送日志复制RPC
void replicate_log(RaftNode *node) {
    for (int i = 0; i < node->peer_count; i++) {
        int peer = node->peers[i];
        uint64_t next_idx = node->next_index[i];

        // 准备请求
        AppendEntriesReq req = {
            .term = node->current_term,
            .leader_id = node->id,
            .prev_log_index = next_idx - 1,
            .prev_log_term = log_term_at(node, next_idx - 1),
            .leader_commit = node->commit_index
        };

        // 收集日志条目
        size_t count = 0;
        for (uint64_t idx = next_idx; idx <= log_last_index(node); idx++) {
            req.entries[count++] = *log_at(node, idx);
            if (count >= MAX_ENTRIES) break;
        }
        req.entry_count = count;

        send_append_entries(peer, &req);
    }
}

// 处理AppendEntries请求
AppendEntriesRsp handle_append_entries(RaftNode *node, AppendEntriesReq *req) {
    AppendEntriesRsp rsp = {0};
    rsp.term = node->current_term;

    // 任期过时，拒绝
    if (req->term < node->current_term) {
        rsp.success = false;
        return rsp;
    }

    // 转换为跟随者
    if (req->term > node->current_term || node->state != STATE_FOLLOWER) {
        become_follower(node, req->term);
    }

    reset_election_timer(node);

    // 检查日志匹配
    if (req->prev_log_index > 0) {
        if (log_last_index(node) < req->prev_log_index) {
            // 日志太短
            rsp.success = false;
            rsp.conflict_index = log_last_index(node);
            return rsp;
        }

        if (log_term_at(node, req->prev_log_index) != req->prev_log_term) {
            // 任期不匹配
            rsp.success = false;
            rsp.conflict_term = log_term_at(node, req->prev_log_index);
            // 找到该任期的第一个索引
            rsp.conflict_index = find_first_index_of_term(node, rsp.conflict_term);
            return rsp;
        }
    }

    // 追加新条目
    for (size_t i = 0; i < req->entry_count; i++) {
        LogEntry *entry = &req->entries[i];
        uint64_t idx = entry->index;

        if (idx <= log_last_index(node)) {
            if (log_term_at(node, idx) != entry->term) {
                // 冲突，删除后续所有条目
                log_truncate(node, idx);
            } else {
                // 已存在，跳过
                continue;
            }
        }

        log_append(node, entry);
    }

    // 更新提交索引
    if (req->leader_commit > node->commit_index) {
        node->commit_index = min(req->leader_commit, log_last_index(node));
        apply_committed(node);
    }

    rsp.success = true;
    return rsp;
}

// 提交日志（当复制到多数节点时）
void check_commit(RaftNode *node) {
    for (uint64_t idx = node->commit_index + 1; idx <= log_last_index(node); idx++) {
        if (log_term_at(node, idx) != node->current_term) {
            continue;  // 只提交当前任期的日志
        }

        // 统计复制到多数节点的日志
        int count = 1;  // 包括自己
        for (int i = 0; i < node->peer_count; i++) {
            if (node->match_index[i] >= idx) {
                count++;
            }
        }

        if (count > (node->peer_count + 1) / 2) {
            node->commit_index = idx;
            apply_committed(node);
        }
    }
}
