/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\README.md
 * Line: 580
 * Language: c
 * Block ID: 24f7c96f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 无锁多生产者多消费者(MPMC)队列
 * 基于DPDK rte_ring的简化实现
 */

typedef struct {
    alignas(CACHE_LINE_SIZE)
    struct {
        atomic_uintptr_t ptr;       /* 数据指针 + 标志位 */
        atomic_uint64_t seq;        /* 序列号 */
    } *ring;

    size_t size;
    size_t mask;

    /* 生产者头尾 - 分开缓存行 */
    alignas(CACHE_LINE_SIZE)
    atomic_size_t prod_head;
    atomic_size_t prod_tail;

    /* 消费者头尾 */
    alignas(CACHE_LINE_SIZE)
    atomic_size_t cons_head;
    atomic_size_t cons_tail;
} mpmc_queue_t;

/* MPMC入队 */
bool mpmc_enqueue(mpmc_queue_t *q, void *item)
{
    size_t prod_head, prod_next;
    size_t cons_tail;
    bool success = false;

    /* 获取生产者位置 */
    do {
        prod_head = atomic_load_explicit(&q->prod_head,
                                          memory_order_relaxed);
        cons_tail = atomic_load_explicit(&q->cons_tail,
                                          memory_order_acquire);

        /* 检查可用空间 */
        if ((prod_head - cons_tail) >= q->size) {
            return false;  /* 队列满 */
        }

        prod_next = prod_head + 1;

        /* CAS更新prod_head */
    } while (!atomic_compare_exchange_weak_explicit(
        &q->prod_head,
        &prod_head, prod_next,
        memory_order_relaxed,
        memory_order_relaxed));

    /* 写入数据 */
    size_t idx = prod_head & q->mask;
    atomic_store_explicit(&q->ring[idx].ptr, (uintptr_t)item,
                          memory_order_relaxed);
    atomic_store_explicit(&q->ring[idx].seq, prod_head + 1,
                          memory_order_release);

    /* 更新prod_tail（可能乱序完成） */
    while (1) {
        prod_head = atomic_load_explicit(&q->prod_head,
                                          memory_order_acquire);
        if (prod_head == prod_next) {
            /* 更新tail */
            atomic_store_explicit(&q->prod_tail, prod_next,
                                  memory_order_release);
            break;
        }
        /* 等待其他生产者完成 */
        cpu_relax();
    }

    return true;
}
