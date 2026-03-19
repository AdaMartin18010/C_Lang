/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\01_LockFree_Ring_Log.md
 * Line: 185
 * Language: c
 * Block ID: a50e7f90
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* SPSC环形缓冲区
 * 分离的读写指针避免缓存行竞争
 */
typedef struct {
    /* 生产者区域 - 独占写入 */
    struct {
        AtomicU64 write_idx CACHE_ALIGNED;    /* 写入位置 */
        uint64_t write_cached_read;            /* 缓存的读位置 */
    } producer;

    /* 消费者区域 - 独占读取 */
    struct {
        AtomicU64 read_idx CACHE_ALIGNED;     /* 读取位置 */
        uint64_t read_cached_write;            /* 缓存的写位置 */
    } consumer;

    /* 缓冲区配置 */
    uint32_t capacity;
    uint32_t mask;

    /* 数据存储 */
    void **buffer;

    /* 可选：预分配对象池 */
    void *pool;
    size_t object_size;
} SPSCRingBuffer;

/* 创建SPSC环形缓冲 */
SPSCRingBuffer* spsc_ring_create(uint32_t capacity, size_t object_size) {
    /* 容量必须是2的幂 */
    if ((capacity & (capacity - 1)) != 0) {
        capacity = 1 << (32 - __builtin_clz(capacity - 1));
    }

    SPSCRingBuffer *rb = aligned_alloc(CACHE_LINE_SIZE, sizeof(SPSCRingBuffer));
    memset(rb, 0, sizeof(SPSCRingBuffer));

    rb->capacity = capacity;
    rb->mask = capacity - 1;
    rb->object_size = object_size;

    /* 分配指针数组 */
    rb->buffer = aligned_alloc(CACHE_LINE_SIZE, capacity * sizeof(void *));

    /* 预分配对象池 */
    if (object_size > 0) {
        rb->pool = aligned_alloc(CACHE_LINE_SIZE, capacity * object_size);
        for (uint32_t i = 0; i < capacity; i++) {
            rb->buffer[i] = (char *)rb->pool + i * object_size;
        }
    }

    return rb;
}
