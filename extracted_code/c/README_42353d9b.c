/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\README.md
 * Line: 666
 * Language: c
 * Block ID: 42353d9b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * HFT内存优化技术
 */

/* 1. 缓存行对齐结构 */
struct __attribute__((aligned(CACHE_LINE_SIZE))) order_message {
    uint64_t timestamp_ns;       /* 时间戳 */
    uint32_t symbol_id;          /* 股票代码 */
    uint32_t order_id;           /* 订单ID */
    int64_t  price;              /* 价格（定点数） */
    uint32_t quantity;           /* 数量 */
    uint8_t  side;               /* 买卖方向 */
    uint8_t  order_type;         /* 订单类型 */
    uint8_t  time_in_force;      /* 有效期 */
    uint8_t  padding[41];        /* 填充到64字节 */
};  /* 总大小: 64字节 = 1个缓存行 */

static_assert(sizeof(struct order_message) == CACHE_LINE_SIZE,
              "order_message must fit in one cache line");

/* 2. 预分配对象池 */
struct object_pool {
    void *memory;
    size_t obj_size;
    size_t capacity;

    /* 空闲链表 - LIFO提高缓存局部性 */
    _Atomic(void *) free_list;

    /* 统计 */
    atomic_size_t alloc_count;
    atomic_size_t free_count;
};

void *pool_alloc(struct object_pool *pool)
{
    void *obj;
    void *next;

    /* 原子弹出空闲链表头 */
    do {
        obj = atomic_load_explicit(&pool->free_list,
                                    memory_order_acquire);
        if (obj == NULL) {
            return NULL;  /* 池耗尽 */
        }
        next = *(void **)obj;  /* 下一个空闲对象 */
    } while (!atomic_compare_exchange_weak_explicit(
        &pool->free_list, &obj, next,
        memory_order_release,
        memory_order_relaxed));

    atomic_fetch_add(&pool->alloc_count, 1);
    return obj;
}

void pool_free(struct object_pool *pool, void *obj)
{
    void *head;

    /* 原子压入空闲链表 */
    do {
        head = atomic_load_explicit(&pool->free_list,
                                     memory_order_relaxed);
        *(void **)obj = head;
    } while (!atomic_compare_exchange_weak_explicit(
        &pool->free_list, &head, obj,
        memory_order_release,
        memory_order_relaxed));

    atomic_fetch_add(&pool->free_count, 1);
}

/* 3. Hugepages内存分配 */
#include <sys/mman.h>
#include <hugetlbfs.h>

void *allocate_hugepages(size_t size, int socket_id)
{
    void *addr;

    /* 使用1GB大页 */
    addr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB |
                MAP_HUGE_1GB | MAP_LOCKED,
                -1, 0);

    if (addr == MAP_FAILED) {
        /* 回退到2MB大页 */
        addr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB |
                    MAP_HUGE_2MB | MAP_LOCKED,
                    -1, 0);
    }

    /* NUMA绑定 */
    if (socket_id >= 0) {
        unsigned long nodemask = 1UL << socket_id;
        mbind(addr, size, MPOL_BIND, &nodemask,
              sizeof(nodemask) * 8, MPOL_MF_MOVE);
    }

    return (addr == MAP_FAILED) ? NULL : addr;
}
