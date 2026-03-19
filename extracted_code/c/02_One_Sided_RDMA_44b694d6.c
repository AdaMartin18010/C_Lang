/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\02_One_Sided_RDMA.md
 * Line: 449
 * Language: c
 * Block ID: 44b694d6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 自适应批处理策略 */
typedef struct {
    int batch_size;
    int max_batch;
    int min_batch;

    uint64_t last_submit;
    uint64_t latency_threshold_us;

    SendBatch batch;
} AdaptiveBatcher;

void adaptive_batch_init(AdaptiveBatcher *ab) {
    ab->batch_size = 0;
    ab->max_batch = 64;
    ab->min_batch = 4;
    ab->latency_threshold_us = 10;
    batch_init(&ab->batch);
}

int adaptive_batch_submit(AdaptiveBatcher *ab, struct ibv_qp *qp) {
    uint64_t now = get_us_timestamp();

    bool should_submit = false;

    /* 条件1：达到最大批大小 */
    if (ab->batch.count >= ab->max_batch) {
        should_submit = true;
    }
    /* 条件2：超时 */
    else if (ab->batch.count > 0 &&
             now - ab->last_submit > ab->latency_threshold_us) {
        should_submit = true;
    }
    /* 条件3：达到最小批大小且无后续操作 */
    else if (ab->batch.count >= ab->min_batch && no_pending_ops()) {
        should_submit = true;
    }

    if (should_submit) {
        int ret = batch_submit(qp, &ab->batch);
        ab->last_submit = now;

        /* 动态调整批大小 */
        if (ret == 0) {
            ab->max_batch = min(ab->max_batch + 1, 256);
        } else {
            ab->max_batch = max(ab->max_batch / 2, ab->min_batch);
        }

        return ret;
    }

    return 0;
}
