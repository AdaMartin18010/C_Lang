/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\02_Leader_Election.md
 * Line: 435
 * Language: c
 * Block ID: ede67000
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 处理投票响应 */
void handle_vote_response(RaftNode *node, RaftMessage *msg) {
    pthread_mutex_lock(&node->lock);

    /* 忽略非candidate或过期任期的响应 */
    if (node->role != ROLE_CANDIDATE || msg->term < node->current_term) {
        pthread_mutex_unlock(&node->lock);
        return;
    }

    /* 更高任期的响应 */
    if (msg->term > node->current_term) {
        become_follower(node, msg->term);
        pthread_mutex_unlock(&node->lock);
        return;
    }

    static uint32_t votes_received = 0;
    static pthread_mutex_t vote_lock = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_lock(&vote_lock);

    if (msg->payload.vote_rsp.vote_granted) {
        votes_received++;
        printf("Node %d received vote, total: %d/%d\n",
               node->node_id, votes_received, node->quorum);

        /* 获得多数派，成为leader */
        if (votes_received >= node->quorum) {
            pthread_mutex_unlock(&vote_lock);
            pthread_mutex_unlock(&node->lock);
            become_leader(node);
            return;
        }
    }

    pthread_mutex_unlock(&vote_lock);
    pthread_mutex_unlock(&node->lock);
}
