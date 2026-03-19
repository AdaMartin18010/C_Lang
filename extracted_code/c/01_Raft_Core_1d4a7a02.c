/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\01_Raft_Core.md
 * Line: 162
 * Language: c
 * Block ID: 1d4a7a02
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 转换为候选人状态
void become_candidate(RaftNode *node) {
    node->state = STATE_CANDIDATE;
    node->current_term++;
    node->voted_for = node->id;

    printf("Node %d becoming candidate for term %lu\n",
           node->id, node->current_term);

    // 重置选举定时器
    reset_election_timer(node);

    // 向所有节点请求投票
    RequestVoteReq req = {
        .term = node->current_term,
        .candidate_id = node->id,
        .last_log_index = log_last_index(node),
        .last_log_term = log_last_term(node)
    };

    for (int i = 0; i < node->peer_count; i++) {
        send_request_vote(node->peers[i], &req);
    }
}

// 处理投票请求
RequestVoteRsp handle_request_vote(RaftNode *node, RequestVoteReq *req) {
    RequestVoteRsp rsp = {.term = node->current_term, .vote_granted = false};

    // 如果任期更高，转换为跟随者
    if (req->term > node->current_term) {
        become_follower(node, req->term);
    }

    // 拒绝条件1：任期过时
    if (req->term < node->current_term) {
        return rsp;
    }

    // 拒绝条件2：已经投票给其他人
    if (node->voted_for != -1 && node->voted_for != req->candidate_id) {
        return rsp;
    }

    // 拒绝条件3：候选人日志不够新
    if (!is_log_up_to_date(node, req->last_log_index, req->last_log_term)) {
        return rsp;
    }

    // 投票
    node->voted_for = req->candidate_id;
    rsp.vote_granted = true;
    reset_election_timer(node);

    return rsp;
}

// 处理投票响应
void handle_request_vote_rsp(RaftNode *node, RequestVoteRsp *rsp) {
    if (rsp->term > node->current_term) {
        become_follower(node, rsp->term);
        return;
    }

    if (node->state != STATE_CANDIDATE) {
        return;
    }

    if (rsp->vote_granted) {
        node->votes_received++;

        // 获得多数票，成为领导者
        if (node->votes_received > (node->peer_count + 1) / 2) {
            become_leader(node);
        }
    }
}

// 转换为领导者
void become_leader(RaftNode *node) {
    node->state = STATE_LEADER;
    printf("Node %d becoming leader for term %lu\n",
           node->id, node->current_term);

    // 初始化领导者状态
    for (int i = 0; i < node->peer_count; i++) {
        node->next_index[i] = log_last_index(node) + 1;
        node->match_index[i] = 0;
    }

    // 立即发送心跳
    send_heartbeats(node);
}
