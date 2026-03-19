/*
 * Auto-generated from: 07_Modern_Toolchain\10_Distributed_Systems\README.md
 * Line: 125
 * Language: c
 * Block ID: f88167d0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 选举超时触发
void on_election_timeout(raft_node_t* node) {
    // 1. 增加任期
    node->current_term++;

    // 2. 转换为候选者
    node->state = STATE_CANDIDATE;
    node->voted_for = node->id;

    // 3. 给自己投票
    int votes_received = 1;

    // 4. 向其他节点发送 RequestVote RPC
    for (int i = 0; i < node->cluster_size; i++) {
        if (i == node->id) continue;

        request_vote_args_t args = {
            .term = node->current_term,
            .candidate_id = node->id,
            .last_log_index = node->log_count - 1,
            .last_log_term = node->log_count > 0 ?
                node->log[node->log_count - 1].term : 0
        };

        send_request_vote(i, &args);
    }

    // 5. 重置选举定时器
    reset_election_timer(node);
}

// 处理 RequestVote RPC
request_vote_reply_t handle_request_vote(raft_node_t* node,
                                         request_vote_args_t* args) {
    request_vote_reply_t reply = { .term = node->current_term };

    // 1. 任期检查
    if (args->term < node->current_term) {
        reply.vote_granted = false;
        return reply;
    }

    // 2. 更新任期
    if (args->term > node->current_term) {
        node->current_term = args->term;
        node->voted_for = -1;
        become_follower(node);
    }

    // 3. 投票条件检查
    bool can_vote = (node->voted_for == -1 ||
                     node->voted_for == args->candidate_id);
    bool log_is_up_to_date = is_log_up_to_date(node, args);

    reply.vote_granted = can_vote && log_is_up_to_date;
    if (reply.vote_granted) {
        node->voted_for = args->candidate_id;
        reset_election_timer(node);
    }

    return reply;
}
