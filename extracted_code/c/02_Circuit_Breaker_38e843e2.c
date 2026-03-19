/*
 * Auto-generated from: 07_Modern_Toolchain\10_Distributed_Systems\02_Circuit_Breaker.md
 * Line: 408
 * Language: c
 * Block ID: 38e843e2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 舱壁隔离: 限制每个服务的资源使用 */

typedef struct {
    char service_name[64];
    int max_concurrent;     /* 最大并发数 */
    int max_queue_size;     /* 队列大小 */

    atomic_int concurrent;  /* 当前并发 */
    atomic_int queued;      /* 队列中等待 */
} bulkhead_t;

bulkhead_t* bulkhead_create(const char* name, int max_concurrent,
                            int max_queue) {
    bulkhead_t* bh = calloc(1, sizeof(bulkhead_t));
    strncpy(bh->service_name, name, sizeof(bh->service_name));
    bh->max_concurrent = max_concurrent;
    bh->max_queue_size = max_queue;
    atomic_init(&bh->concurrent, 0);
    atomic_init(&bh->queued, 0);
    return bh;
}

/* 尝试进入 */
typedef enum {
    BULKHEAD_ALLOW,     /* 允许立即执行 */
    BULKHEAD_QUEUE,     /* 需要排队 */
    BULKHEAD_REJECT     /* 拒绝 */
} bulkhead_result_t;

bulkhead_result_t bulkhead_enter(bulkhead_t* bh) {
    int current = atomic_load(&bh->concurrent);

    if (current < bh->max_concurrent) {
        /* 有空闲槽 */
        if (atomic_compare_exchange_strong(&bh->concurrent, &current,
                                           current + 1)) {
            return BULKHEAD_ALLOW;
        }
    }

    /* 检查队列 */
    int queued = atomic_fetch_add(&bh->queued, 1);
    if (queued < bh->max_queue_size) {
        return BULKHEAD_QUEUE;
    } else {
        atomic_fetch_sub(&bh->queued, 1);
        return BULKHEAD_REJECT;
    }
}

void bulkhead_exit(bulkhead_t* bh) {
    atomic_fetch_sub(&bh->concurrent, 1);
}

void bulkhead_dequeue(bulkhead_t* bh) {
    atomic_fetch_sub(&bh->queued, 1);
}
