/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\02_DMA_Offload.md
 * Line: 212
 * Language: c
 * Block ID: 5f5379c6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 环形缓冲区实现 - 用于5G基带DMA数据传输
 * 支持多生产者-单消费者模式
 * ============================================================================ */

#include <stdatomic.h>
#include <stdbool.h>

/* 环形缓冲区配置 */
#define DMA_RING_SIZE           1024    /* 必须是2的幂 */
#define DMA_RING_MASK           (DMA_RING_SIZE - 1)
#define DMA_RING_BATCH_SIZE     16      /* 批量处理大小 */

/* 环形缓冲区结构 */
typedef struct dma_ring_buffer {
    dma_descriptor_t *desc_array;       /* 描述符数组 */
    atomic_uint producer_idx;           /* 生产者索引 */
    atomic_uint consumer_idx;           /* 消费者索引 */

    /* 统计信息 */
    atomic_ulong enqueue_count;         /* 入队计数 */
    atomic_ulong dequeue_count;         /* 出队计数 */
    atomic_ulong drop_count;            /* 丢弃计数 */

    /* 同步原语 */
    atomic_flag lock;                   /* 轻量级锁 */
    void *notify_ctx;                   /* 通知上下文 */
    void (*notify_cb)(void *ctx);       /* 通知回调 */
} dma_ring_buffer_t;

/* 初始化环形缓冲区 */
static inline int dma_ring_init(dma_ring_buffer_t *ring,
                                 dma_descriptor_t *desc_array,
                                 void (*notify)(void *), void *ctx)
{
    if (!ring || !desc_array) {
        return -1;
    }

    ring->desc_array = desc_array;
    atomic_init(&ring->producer_idx, 0);
    atomic_init(&ring->consumer_idx, 0);
    atomic_init(&ring->enqueue_count, 0);
    atomic_init(&ring->dequeue_count, 0);
    atomic_init(&ring->drop_count, 0);
    atomic_flag_clear(&ring->lock);
    ring->notify_cb = notify;
    ring->notify_ctx = ctx;

    /* 初始化所有描述符 */
    for (int i = 0; i < DMA_RING_SIZE; i++) {
        desc_array[i].ctrl = 0;
        desc_array[i].status = 0;
        desc_array[i].next_desc_phys = 0;
    }

    return 0;
}

/* 计算环形缓冲区已用空间 */
static inline uint32_t dma_ring_used(const dma_ring_buffer_t *ring)
{
    uint32_t prod = atomic_load_explicit(&ring->producer_idx,
                                          memory_order_acquire);
    uint32_t cons = atomic_load_explicit(&ring->consumer_idx,
                                          memory_order_acquire);
    return (prod - cons) & DMA_RING_MASK;
}

/* 计算环形缓冲区空闲空间 */
static inline uint32_t dma_ring_free(const dma_ring_buffer_t *ring)
{
    return DMA_RING_SIZE - 1 - dma_ring_used(ring);
}

/* 批量入队描述符 */
static inline uint32_t dma_ring_enqueue_batch(dma_ring_buffer_t *ring,
                                               dma_descriptor_t *descs,
                                               uint32_t count)
{
    uint32_t prod = atomic_load_explicit(&ring->producer_idx,
                                          memory_order_relaxed);
    uint32_t free_slots = DMA_RING_SIZE - 1 -
                          ((prod - atomic_load_explicit(&ring->consumer_idx,
                             memory_order_acquire)) & DMA_RING_MASK);

    uint32_t to_enqueue = (count < free_slots) ? count : free_slots;

    for (uint32_t i = 0; i < to_enqueue; i++) {
        uint32_t idx = (prod + i) & DMA_RING_MASK;
        ring->desc_array[idx] = descs[i];

        /* 设置链表指针，除非是最后一个 */
        if (i < to_enqueue - 1) {
            ring->desc_array[idx].next_desc_phys =
                (uint64_t)&ring->desc_array[(idx + 1) & DMA_RING_MASK];
        }
    }

    /* 内存屏障确保描述符写入完成 */
    atomic_thread_fence(memory_order_release);

    atomic_store_explicit(&ring->producer_idx,
                          (prod + to_enqueue) & (DMA_RING_SIZE * 2 - 1),
                          memory_order_release);

    atomic_fetch_add_explicit(&ring->enqueue_count, to_enqueue,
                              memory_order_relaxed);

    /* 触发通知 */
    if (ring->notify_cb && to_enqueue > 0) {
        ring->notify_cb(ring->notify_ctx);
    }

    return to_enqueue;
}
