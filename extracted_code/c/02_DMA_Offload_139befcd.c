/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\02_DMA_Offload.md
 * Line: 1160
 * Language: c
 * Block ID: 139befcd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 完成通知机制 - 支持轮询、信号量和完成队列
 * ============================================================================ */

typedef enum {
    NOTIFY_POLLING = 0,                 /* 轮询模式 */
    NOTIFY_SIGNAL,                      /* 信号量模式 */
    NOTIFY_CALLBACK,                    /* 回调模式 */
    NOTIFY_COMPLETION_QUEUE             /* 完成队列模式 */
} notify_mode_t;

/* 完成事件 */
typedef struct completion_event {
    uint32_t ch_id;                     /* 通道ID */
    uint64_t desc_phys;                 /* 描述符物理地址 */
    uint32_t status;                    /* 状态 */
    uint64_t timestamp;                 /* 时间戳 */
    uint64_t xfer_bytes;                /* 传输字节数 */
} completion_event_t;

/* 完成队列 */
typedef struct completion_queue {
    completion_event_t *events;         /* 事件数组 */
    atomic_uint head;                   /* 头索引 */
    atomic_uint tail;                   /* 尾索引 */
    uint32_t size;                      /* 队列大小 */
    atomic_flag lock;
} completion_queue_t;

/* 初始化完成队列 */
int comp_queue_init(completion_queue_t *cq, uint32_t size)
{
    if (!cq || size == 0) return -1;

    cq->events = calloc(size, sizeof(completion_event_t));
    if (!cq->events) return -1;

    cq->size = size;
    atomic_init(&cq->head, 0);
    atomic_init(&cq->tail, 0);
    atomic_flag_clear(&cq->lock);

    return 0;
}

/* 入队完成事件 */
bool comp_queue_enqueue(completion_queue_t *cq, const completion_event_t *evt)
{
    if (!cq || !evt) return false;

    uint32_t tail = atomic_load_explicit(&cq->tail, memory_order_relaxed);
    uint32_t next_tail = (tail + 1) % cq->size;

    if (next_tail == atomic_load_explicit(&cq->head, memory_order_acquire)) {
        return false;  /* 队列满 */
    }

    cq->events[tail] = *evt;
    atomic_thread_fence(memory_order_release);
    atomic_store_explicit(&cq->tail, next_tail, memory_order_release);

    return true;
}

/* 出队完成事件 */
bool comp_queue_dequeue(completion_queue_t *cq, completion_event_t *evt)
{
    if (!cq || !evt) return false;

    uint32_t head = atomic_load_explicit(&cq->head, memory_order_relaxed);

    if (head == atomic_load_explicit(&cq->tail, memory_order_acquire)) {
        return false;  /* 队列空 */
    }

    *evt = cq->events[head];
    atomic_thread_fence(memory_order_release);
    atomic_store_explicit(&cq->head, (head + 1) % cq->size,
                          memory_order_release);

    return true;
}

/* 等待完成 (带超时) */
int dma_wait_for_completion(dma_channel_t *ch, uint32_t timeout_ms)
{
    if (!ch) return -1;

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    uint64_t end_time = (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000
                        + timeout_ms;

    while (ch->state == DMA_CH_STATE_ACTIVE) {
        /* 轮询状态 */
        uint32_t status = REG_READ32(DMA_CH_STATUS(ch->id));

        if (status & (CH_STAT_CHAIN_DONE | CH_STAT_ERROR)) {
            break;
        }

        clock_gettime(CLOCK_MONOTONIC, &ts);
        uint64_t now = (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

        if (now >= end_time) {
            return -1;  /* 超时 */
        }

        /* 短暂让步CPU */
        __asm__ __volatile__("yield");
    }

    return (ch->state == DMA_CH_STATE_IDLE) ? 0 : -1;
}
