/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\03_Lockless_Ring_Buffer_SPSC_MPMC.md
 * Line: 169
 * Language: c
 * Block ID: 27d69945
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// MPMC无锁队列（基于DPDK rte_ring）
#define MP_MC_BUF_MASK  (RTE_RING_SZ_MASK)
#define MP_MC_BUF_FLAG  (1ULL << 63)  // 预留位

typedef struct {
    _Atomic uint64_t prod_head;
    _Atomic uint64_t prod_tail;
    char _pad1[CACHE_LINE_SIZE - 2 * sizeof(_Atomic uint64_t)];

    _Atomic uint64_t cons_head;
    _Atomic uint64_t cons_tail;
    char _pad2[CACHE_LINE_SIZE - 2 * sizeof(_Atomic uint64_t)];

    void *ring[];  // 柔性数组
} MPMCRing;

// 多生产者入队（批量优化）
size_t mpmc_enqueue(MPMCRing *r, void * const *obj_table, size_t n) {
    uint64_t prod_head, prod_next;
    uint64_t cons_tail;
    size_t free_entries;

    // 移动生产者头（需要CAS重试）
    do {
        prod_head = atomic_load_explicit(&r->prod_head, memory_order_relaxed);
        cons_tail = atomic_load_explicit(&r->cons_tail, memory_order_relaxed);

        // 计算空闲空间
        free_entries = (cons_tail + r->capacity - prod_head - 1);

        if (n > free_entries) {
            // 队列满或请求太大
            if (free_entries == 0) return 0;
            n = free_entries;
        }

        prod_next = prod_head + n;

        // CAS更新head
    } while (!atomic_compare_exchange_weak_explicit(
        &r->prod_head, &prod_head, prod_next,
        memory_order_relaxed, memory_order_relaxed));

    // 写入对象指针
    for (size_t i = 0; i < n; i++) {
        r->ring[(prod_head + i) & r->mask] = obj_table[i];
    }

    // 等待其他生产者完成（DPDK的"wait for tail"优化）
    while (atomic_load_explicit(&r->prod_tail, memory_order_relaxed) != prod_head) {
        // 忙等或pause指令
        __asm__ volatile ("pause" ::: "memory");
    }

    // 更新tail
    atomic_store_explicit(&r->prod_tail, prod_next, memory_order_release);

    return n;
}

// 多消费者出队
size_t mpmc_dequeue(MPMCRing *r, void **obj_table, size_t n) {
    uint64_t cons_head, cons_next;
    uint64_t prod_tail;
    size_t entries;

    // 移动消费者头
    do {
        cons_head = atomic_load_explicit(&r->cons_head, memory_order_relaxed);
        prod_tail = atomic_load_explicit(&r->prod_tail, memory_order_acquire);

        entries = prod_tail - cons_head;
        if (entries == 0) return 0;

        if (n > entries) n = entries;
        cons_next = cons_head + n;

    } while (!atomic_compare_exchange_weak_explicit(
        &r->cons_head, &cons_head, cons_next,
        memory_order_relaxed, memory_order_relaxed));

    // 读取对象指针
    for (size_t i = 0; i < n; i++) {
        obj_table[i] = r->ring[(cons_head + i) & r->mask];
    }

    // 等待其他消费者
    while (atomic_load_explicit(&r->cons_tail, memory_order_relaxed) != cons_head) {
        __asm__ volatile ("pause" ::: "memory");
    }

    atomic_store_explicit(&r->cons_tail, cons_next, memory_order_release);

    return n;
}
