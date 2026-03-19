/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\README.md
 * Line: 599
 * Language: c
 * Block ID: 07f53b36
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* PBFT 消息类型 */
typedef enum {
    PBFT_MSG_REQUEST = 1,
    PBFT_MSG_PRE_PREPARE,
    PBFT_MSG_PREPARE,
    PBFT_MSG_COMMIT,
    PBFT_MSG_VIEW_CHANGE,
    PBFT_MSG_NEW_VIEW,
    PBFT_MSG_CHECKPOINT
} pbft_msg_type_t;

/* PBFT 请求消息 */
typedef struct {
    uint8_t operation[OPERATION_MAX_SIZE];  /* 操作内容 */
    uint64_t timestamp;                      /* 客户端时间戳 */
    int client_id;                           /* 客户端ID */
    uint8_t signature[SIGNATURE_SIZE];       /* 客户端签名 */
} pbft_request_t;

/* Pre-Prepare消息 */
typedef struct {
    int view;                               /* 视图编号 */
    uint64_t sequence;                       /* 序列号 */
    uint8_t digest[DIGEST_SIZE];            /* 请求摘要 */
    pbft_request_t request;                 /* 原始请求 */
    int replica_id;                         /* 主副本ID */
    uint8_t signature[SIGNATURE_SIZE];      /* 主副本签名 */
} pbft_pre_prepare_t;

/* Prepare消息 */
typedef struct {
    int view;
    uint64_t sequence;
    uint8_t digest[DIGEST_SIZE];
    int replica_id;
    uint8_t signature[SIGNATURE_SIZE];
} pbft_prepare_t;

/* Commit消息 */
typedef struct {
    int view;
    uint64_t sequence;
    uint8_t digest[DIGEST_SIZE];
    int replica_id;
    uint8_t signature[SIGNATURE_SIZE];
} pbft_commit_t;

/* 副本状态 */
typedef struct {
    int replica_id;
    int view;                               /* 当前视图 */
    uint64_t sequence;                       /* 当前序列号 */
    int f;                                  /* 容错数 */
    int n;                                  /* 总节点数 */

    /* 日志 */
    pbft_log_entry_t *log;

    /* 状态追踪 */
    /* 对每个<view, sequence>追踪收到的prepare/commit */
    map_t prepared_certificates;            /* 达到prepared状态的证明 */
    map_t committed_certificates;           /* 达到committed状态的证明 */

    /* 视图变更 */
    bool view_change_in_progress;
    map_t view_change_messages;             /* 收集的VIEW-CHANGE消息 */

    /* 检查点 */
    uint64_t last_stable_checkpoint;

    /* 加密材料 */
    uint8_t private_key[PRIVATE_KEY_SIZE];
    uint8_t public_keys[MAX_REPLICAS][PUBLIC_KEY_SIZE];
} pbft_replica_t;

/* PBFT 主流程 */
void pbft_handle_request(pbft_replica_t *replica, pbft_request_t *req) {
    if (!is_primary(replica)) {
        /* 转发给主副本 */
        forward_to_primary(replica, req);
        return;
    }

    /* 验证客户端签名 */
    if (!verify_client_signature(req)) {
        return;
    }

    /* 分配序列号 */
    uint64_t seq = ++replica->sequence;

    /* 计算请求摘要 */
    uint8_t digest[DIGEST_SIZE];
    hash_request(req, digest);

    /* 创建并广播Pre-Prepare */
    pbft_pre_prepare_t pre_prepare = {
        .view = replica->view,
        .sequence = seq,
        .replica_id = replica->replica_id
    };
    memcpy(pre_prepare.digest, digest, DIGEST_SIZE);
    memcpy(&pre_prepare.request, req, sizeof(*req));
    sign_message(&pre_prepare, sizeof(pre_prepare) - SIGNATURE_SIZE,
                 replica->private_key, pre_prepare.signature);

    broadcast_pre_prepare(replica, &pre_prepare);

    /* 自己添加到pre-prepared状态 */
    add_to_pre_prepared(replica, replica->view, seq, digest);
}

void pbft_handle_pre_prepare(pbft_replica_t *replica,
                             pbft_pre_prepare_t *pp) {
    /* 验证条件检查 */
    if (pp->view != replica->view) return;
    if (pp->sequence <= replica->last_stable_checkpoint) return;
    if (pp->sequence > replica->last_stable_checkpoint + WATER_MARK) return;

    /* 验证摘要 */
    uint8_t computed_digest[DIGECT_SIZE];
    hash_request(&pp->request, computed_digest);
    if (memcmp(computed_digest, pp->digest, DIGEST_SIZE) != 0) return;

    /* 验证主副本签名 */
    int primary = get_primary(pp->view, replica->n);
    if (!verify_signature(pp, sizeof(*pp) - SIGNATURE_SIZE,
                          pp->signature, replica->public_keys[primary])) {
        return;
    }

    /* 进入pre-prepared状态 */
    add_to_pre_prepared(replica, pp->view, pp->sequence, pp->digest);

    /* 发送Prepare消息 */
    pbft_prepare_t prepare = {
        .view = replica->view,
        .sequence = pp->sequence,
        .replica_id = replica->replica_id
    };
    memcpy(prepare.digest, pp->digest, DIGEST_SIZE);
    sign_message(&prepare, sizeof(prepare) - SIGNATURE_SIZE,
                 replica->private_key, prepare.signature);

    broadcast_prepare(replica, &prepare);
}

void pbft_handle_prepare(pbft_replica_t *replica, pbft_prepare_t *p) {
    if (p->view != replica->view) return;

    /* 验证签名 */
    if (!verify_replica_signature(replica, p->replica_id,
                                   p, sizeof(*p) - SIGNATURE_SIZE,
                                   p->signature)) {
        return;
    }

    /* 收集prepare */
    if (!add_prepare_vote(replica, p->view, p->sequence,
                          p->digest, p->replica_id)) {
        return;  /* 重复或无效 */
    }

    /* 检查是否达到prepared状态（2f个prepare） */
    if (is_prepared(replica, p->view, p->sequence, p->digest)) {
        /* 发送Commit */
        pbft_commit_t commit = {
            .view = replica->view,
            .sequence = p->sequence,
            .replica_id = replica->replica_id
        };
        memcpy(commit.digest, p->digest, DIGEST_SIZE);
        sign_message(&commit, sizeof(commit) - SIGNATURE_SIZE,
                     replica->private_key, commit.signature);

        broadcast_commit(replica, &commit);
    }
}

void pbft_handle_commit(pbft_replica_t *replica, pbft_commit_t *c) {
    if (c->view != replica->view) return;

    /* 验证签名 */
    if (!verify_replica_signature(replica, c->replica_id,
                                   c, sizeof(*c) - SIGNATURE_SIZE,
                                   c->signature)) {
        return;
    }

    /* 收集commit */
    if (!add_commit_vote(replica, c->view, c->sequence,
                         c->digest, c->replica_id)) {
        return;
    }

    /* 检查是否达到committed状态（2f+1个commit） */
    if (is_committed(replica, c->view, c->sequence, c->digest)) {
        /* 执行操作并回复客户端 */
        execute_request(replica, c->view, c->sequence);
    }
}
