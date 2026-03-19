/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\README.md
 * Line: 224
 * Language: c
 * Block ID: 7d24b1c6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 处理选举超时 - 转换为候选人 */
void raft_handle_election_timeout(raft_node_t *node) {
    uint64_t now = get_monotonic_ms();

    if (node->state == RAFT_STATE_LEADER)
        return;  /* 领导者不会超时 */

    if (now - node->last_heartbeat < node->election_timeout)
        return;  /* 未到超时时间 */

    /* 转换为候选人 */
    raft_become_candidate(node);
}

/* 转换为候选人 */
void raft_become_candidate(raft_node_t *node) {
    node->state = RAFT_STATE_CANDIDATE;
    node->current_term++;
    node->voted_for = node->node_id;  /* 给自己投票 */
    node->last_heartbeat = get_monotonic_ms();

    /* 随机化选举超时（避免活锁） */
    node->election_timeout = ELECTION_TIMEOUT_MIN +
        (rand() % (ELECTION_TIMEOUT_MAX - ELECTION_TIMEOUT_MIN));

    raft_persist_state(node);  /* 持久化任期和投票 */

    /* 发送RequestVote RPC给所有节点 */
    request_vote_req_t req = {
        .term = node->current_term,
        .candidate_id = node->node_id,
        .last_log_index = raft_get_last_log_index(node),
        .last_log_term = raft_get_last_log_term(node)
    };

    int votes = 1;  /* 自己的票 */
    for (int i = 0; i < node->node_count; i++) {
        if (node->peer_ids[i] == node->node_id) continue;

        raft_send_rpc(node, node->peer_ids[i],
                      RAFT_MSG_REQUEST_VOTE, &req, sizeof(req));
    }
}

/* 处理RequestVote RPC */
void raft_handle_request_vote(raft_node_t *node, int from,
                              request_vote_req_t *req,
                              request_vote_resp_t *resp) {
    resp->term = node->current_term;
    resp->vote_granted = false;

    /* 1. 如果候选人任期更低，拒绝 */
    if (req->term < node->current_term) {
        return;
    }

    /* 2. 如果收到更高任期，转换为跟随者 */
    if (req->term > node->current_term) {
        raft_become_follower(node, req->term);
    }

    /* 3. 投票条件检查 */
    bool can_vote = (node->voted_for == -1 ||
                     node->voted_for == req->candidate_id);

    /* 4. 日志新鲜度检查 - 候选人的日志必须至少和自己一样新 */
    bool log_is_up_to_date =
        (req->last_log_term > raft_get_last_log_term(node)) ||
        (req->last_log_term == raft_get_last_log_term(node) &&
         req->last_log_index >= raft_get_last_log_index(node));

    if (can_vote && log_is_up_to_date) {
        resp->vote_granted = true;
        node->voted_for = req->candidate_id;
        node->last_heartbeat = get_monotonic_ms();  /* 重置超时 */
        raft_persist_state(node);
    }
}

/* 处理RequestVote响应 */
void raft_handle_request_vote_response(raft_node_t *node, int from,
                                       request_vote_resp_t *resp) {
    /* 如果收到更高任期，转换为跟随者 */
    if (resp->term > node->current_term) {
        raft_become_follower(node, resp->term);
        return;
    }

    /* 只处理候选人在当前任期的响应 */
    if (node->state != RAFT_STATE_CANDIDATE ||
        resp->term != node->current_term) {
        return;
    }

    static int votes_received = 1;  /* 已在become_candidate中初始化 */
    if (resp->vote_granted) {
        votes_received++;
    }

    /* 获得多数票，成为领导者 */
    if (votes_received > node->node_count / 2) {
        raft_become_leader(node);
    }
}
