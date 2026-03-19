/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\02_Leader_Election.md
 * Line: 332
 * Language: c
 * Block ID: 0be1784c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 发起选举 - RequestVote RPC */
void start_election(RaftNode *node) {
    pthread_mutex_lock(&node->lock);

    /* 获取最后日志信息 */
    uint32_t last_log_index = 0;
    uint64_t last_log_term = 0;

    if (node->log_count > 0) {
        LogEntry *last = &node->log[node->log_count - 1];
        last_log_index = last->index;
        last_log_term = last->term;
    }

    uint64_t term = node->current_term;
    uint32_t votes_received = 1;  /* 自己的票 */

    pthread_mutex_unlock(&node->lock);

    printf("Node %d starting election for term %lu\n", node->node_id, term);

    /* 向所有peer发送RequestVote */
    for (uint32_t i = 0; i < node->peer_count; i++) {
        RaftMessage msg = {
            .type = MSG_VOTE_REQUEST,
            .from = node->node_id,
            .to = node->peers[i],
            .term = term,
            .payload.vote_req = {
                .last_log_index = last_log_index,
                .last_log_term = last_log_term,
            }
        };

        /* 异步发送（实际实现需要网络层） */
        send_message_async(node, &msg);
    }
}

/* 处理投票请求 */
void handle_vote_request(RaftNode *node, RaftMessage *msg) {
    pthread_mutex_lock(&node->lock);

    RaftMessage response = {
        .type = MSG_VOTE_RESPONSE,
        .from = node->node_id,
        .to = msg->from,
        .term = node->current_term,
        .payload.vote_rsp.vote_granted = false,
    };

    /* 任期检查 */
    if (msg->term < node->current_term) {
        /* 拒绝：请求者任期旧 */
        response.term = node->current_term;
        pthread_mutex_unlock(&node->lock);
        send_message(node, &response);
        return;
    }

    if (msg->term > node->current_term) {
        /* 转换为follower */
        node->current_term = msg->term;
        node->voted_for = 0;
        if (node->role != ROLE_FOLLOWER) {
            become_follower(node, msg->term);
        }
    }

    /* 选举限制：日志比较 */
    bool log_ok = false;
    uint32_t my_last_index = node->log_count > 0 ?
                             node->log[node->log_count - 1].index : 0;
    uint64_t my_last_term = node->log_count > 0 ?
                            node->log[node->log_count - 1].term : 0;

    uint32_t their_last_index = msg->payload.vote_req.last_log_index;
    uint64_t their_last_term = msg->payload.vote_req.last_log_term;

    if (their_last_term > my_last_term) {
        log_ok = true;
    } else if (their_last_term == my_last_term &&
               their_last_index >= my_last_index) {
        log_ok = true;
    }

    /* 投票决策 */
    if ((node->voted_for == 0 || node->voted_for == msg->from) && log_ok) {
        node->voted_for = msg->from;
        response.payload.vote_rsp.vote_granted = true;
        reset_election_timeout(node);
        printf("Node %d voted for %d at term %lu\n",
               node->node_id, msg->from, node->current_term);
    }

    pthread_mutex_unlock(&node->lock);
    send_message(node, &response);
}
