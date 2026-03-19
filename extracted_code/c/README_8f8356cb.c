/*
 * Auto-generated from: 07_Modern_Toolchain\10_Distributed_Systems\README.md
 * Line: 192
 * Language: c
 * Block ID: 8f8356cb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 客户端请求处理 (仅领导者)
int raft_append_entry(raft_node_t* node, const char* data, size_t len) {
    if (node->state != STATE_LEADER) {
        return -1;  // 非领导者，重定向
    }

    // 1. 创建日志条目
    log_entry_t entry = {
        .term = node->current_term,
        .index = node->log_count,
        .data = malloc(len),
        .data_len = len
    };
    memcpy(entry.data, data, len);

    // 2. 追加到本地日志
    node->log[node->log_count++] = entry;

    // 3. 并行发送 AppendEntries RPC
    for (int i = 0; i < node->cluster_size; i++) {
        if (i == node->id) continue;
        send_append_entries(node, i);
    }

    return entry.index;
}

// 处理 AppendEntries RPC
append_entries_reply_t handle_append_entries(raft_node_t* node,
                                             append_entries_args_t* args) {
    append_entries_reply_t reply = {
        .term = node->current_term,
        .success = false
    };

    // 1. 任期检查
    if (args->term < node->current_term) {
        return reply;
    }

    // 2. 成为跟随者并重置定时器
    if (args->term > node->current_term) {
        node->current_term = args->term;
    }
    become_follower(node);
    reset_election_timer(node);

    // 3. 日志一致性检查
    if (args->prev_log_index >= 0) {
        if (args->prev_log_index >= node->log_count ||
            node->log[args->prev_log_index].term != args->prev_log_term) {
            // 日志不一致，需要回溯
            reply.conflict_index = find_conflict_index(node, args);
            return reply;
        }
    }

    // 4. 追加新条目
    int insert_index = args->prev_log_index + 1;
    for (int i = 0; i < args->entry_count; i++) {
        if (insert_index + i < node->log_count) {
            if (node->log[insert_index + i].term != args->entries[i].term) {
                // 删除冲突条目及之后的所有条目
                truncate_log(node, insert_index + i);
            }
        }
        if (insert_index + i >= node->log_count) {
            append_log_entry(node, &args->entries[i]);
        }
    }

    // 5. 更新提交索引
    if (args->leader_commit > node->commit_index) {
        node->commit_index = min(args->leader_commit, node->log_count - 1);
        apply_committed_entries(node);
    }

    reply.success = true;
    return reply;
}
