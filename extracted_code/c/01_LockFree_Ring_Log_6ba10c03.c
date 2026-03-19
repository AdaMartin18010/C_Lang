/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\01_LockFree_Ring_Log.md
 * Line: 335
 * Language: c
 * Block ID: 6ba10c03
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* MPMC队列节点状态 */
typedef enum {
    ENTRY_EMPTY = 0,    /* 空槽位 */
    ENTRY_BUSY,         /* 正在写入 */
    ENTRY_READY,        /* 数据就绪 */
    ENTRY_CONSUMING,    /* 正在读取 */
} EntryState;

typedef struct {
    _Atomic(uint64_t) seq;      /* 序列号 */
    _Atomic(void *) data;       /* 数据指针 */
} RingEntry;

/* MPMC环形缓冲区 */
typedef struct {
    AtomicU64 write_seq CACHE_ALIGNED;   /* 生产者序列 */
    AtomicU64 read_seq CACHE_ALIGNED;    /* 消费者序列 */

    uint32_t capacity;
    uint32_t mask;

    RingEntry *entries;  /* 条目数组 */
} MPMCRingBuffer;

/* 创建MPMC缓冲区 */
MPMCRingBuffer* mpmc_ring_create(uint32_t capacity) {
    if ((capacity & (capacity - 1)) != 0) {
        capacity = 1 << (32 - __builtin_clz(capacity - 1));
    }

    MPMCRingBuffer *rb = aligned_alloc(CACHE_LINE_SIZE, sizeof(MPMCRingBuffer));
    rb->capacity = capacity;
    rb->mask = capacity - 1;

    rb->entries = aligned_alloc(CACHE_LINE_SIZE, capacity * sizeof(RingEntry));

    /* 初始化序列号 */
    for (uint32_t i = 0; i < capacity; i++) {
        atomic_init(&rb->entries[i].seq, i);
        atomic_init(&rb->entries[i].data, NULL);
    }

    atomic_init(&rb->write_seq.value, 0);
    atomic_init(&rb->read_seq.value, 0);

    return rb;
}

/* MPMC写入 - 基于序列号 */
bool mpmc_ring_push(MPMCRingBuffer *rb, void *data) {
    RingEntry *entry;
    uint64_t seq;
    uint32_t pos;

    /* 获取写序列号 */
    uint64_t write_seq = atomic_fetch_add_explicit(
        &rb->write_seq.value, 1, memory_order_relaxed);

retry:
    pos = write_seq & rb->mask;
    entry = &rb->entries[pos];
    seq = atomic_load_explicit(&entry->seq, memory_order_acquire);

    int64_t diff = (int64_t)seq - (int64_t)write_seq;

    if (diff == 0) {
        /* 尝试占用槽位 */
        if (!atomic_compare_exchange_weak_explicit(
                &entry->seq, &seq, write_seq + 1,
                memory_order_release, memory_order_relaxed)) {
            goto retry;
        }

        /* 写入数据 */
        atomic_store_explicit(&entry->data, data, memory_order_release);
        return true;
    } else if (diff < 0) {
        /* 队列满 */
        /* 回退write_seq并返回失败 */
        return false;
    } else {
        /* 被其他生产者抢占，重试 */
        goto retry;
    }
}

/* MPMC读取 */
bool mpmc_ring_pop(MPMCRingBuffer *rb, void **data) {
    RingEntry *entry;
    uint64_t seq;
    uint32_t pos;

    /* 获取读序列号 */
    uint64_t read_seq = atomic_fetch_add_explicit(
        &rb->read_seq.value, 1, memory_order_relaxed);

retry:
    pos = read_seq & rb->mask;
    entry = &rb->entries[pos];
    seq = atomic_load_explicit(&entry->seq, memory_order_acquire);

    int64_t diff = (int64_t)seq - (int64_t)(read_seq + 1);

    if (diff == 0) {
        /* 读取数据 */
        *data = atomic_load_explicit(&entry->data, memory_order_acquire);

        /* 更新序列号，标记为空 */
        uint64_t next_seq = read_seq + rb->capacity;
        atomic_store_explicit(&entry->seq, next_seq, memory_order_release);

        return true;
    } else if (diff < 0) {
        /* 队列空 */
        return false;
    } else {
        /* 数据未就绪，重试 */
        goto retry;
    }
}
