# 高级内存技术

> **层级定位**: 01 Core Knowledge System / 02 Core Layer
> **难度级别**: L4-L5
> **预估学习时间**: 8-10 小时

---

## 1. 内存分配器原理

### 1.1 ptmalloc2架构

```
ptmalloc2 (glibc默认分配器)
├── Arena (分配区)
│   ├── 主Arena (main arena) - 主线程使用
│   └── 线程Arena - 每个线程独立
│
├── Heap (堆)
│   ├── 由mmap分配的大块内存
│   └── 按区域管理
│
├── Chunk (内存块)
│   ├── 已分配块 (allocated chunk)
│   └── 空闲块 (free chunk) - 由bins管理
│
└── Bins (空闲块容器)
    ├── Fast bins - 小内存快速分配 (≤ 80 bytes)
    ├── Small bins - 精确匹配 (≤ 1008 bytes)
    ├── Large bins - 范围匹配 (> 1008 bytes)
    └── Unsorted bin - 临时存放
```

### 1.2 Chunk结构

```c
// 已分配块
struct malloc_chunk {
    size_t      mchunk_prev_size;  // 物理相邻前块大小（如果前块空闲）
    size_t      mchunk_size;       // 本块大小 + 标志位

    // 用户数据从这里开始
};

// 空闲块额外字段
struct malloc_chunk_free {
    size_t      mchunk_prev_size;
    size_t      mchunk_size;

    struct malloc_chunk* fd;       // 前向指针 (forward)
    struct malloc_chunk* bk;       // 后向指针 (backward)

    //  LARGE bin还有额外指针
    struct malloc_chunk* fd_nextsize;
    struct malloc_chunk* bk_nextsize;
};

// 大小计算
// chunk_size = (用户请求大小 + 16) 对齐到16字节
```

### 1.3 分配策略

```c
void* malloc(size_t size) {
    // 1. 检查size是否为0
    // 2. 对齐size到16字节

    // 3. 根据大小选择分配路径
    if (size <= 0x80) {
        // Fast bin路径
        return fastbin_alloc(size);
    } else if (size <= 0x3f0) {
        // Small bin路径
        return smallbin_alloc(size);
    } else {
        // Large bin或mmap路径
        return largebin_or_mmap_alloc(size);
    }
}
```

---

## 2. 自定义内存池

### 2.1 固定大小内存池

```c
typedef struct PoolBlock {
    struct PoolBlock *next;
    char data[0];  // 柔性数组
} PoolBlock;

typedef struct {
    size_t block_size;
    size_t blocks_per_chunk;
    PoolBlock *free_list;
    void *chunks;  // 已分配的内存块链表
} MemoryPool;

// 创建内存池
MemoryPool* pool_create(size_t obj_size, size_t count) {
    MemoryPool *pool = malloc(sizeof(MemoryPool));
    pool->block_size = align_to(obj_size + sizeof(PoolBlock*), 8);
    pool->blocks_per_chunk = count;
    pool->free_list = NULL;
    pool->chunks = NULL;

    // 预分配第一批
    pool_grow(pool);
    return pool;
}

// 从池分配
void* pool_alloc(MemoryPool *pool) {
    if (!pool->free_list) {
        pool_grow(pool);
    }

    PoolBlock *block = pool->free_list;
    pool->free_list = block->next;
    return block->data;
}

// 归还到池
void pool_free(MemoryPool *pool, void *ptr) {
    PoolBlock *block = (PoolBlock *)((char *)ptr - offsetof(PoolBlock, data));
    block->next = pool->free_list;
    pool->free_list = block;
}
```

### 2.2 对象池模板

```c
#define DEFINE_POOL_TYPE(TypeName, ElementType, PoolSize) \
    typedef struct { \
        ElementType elements[PoolSize]; \
        int free_list[PoolSize]; \
        int free_count; \
    } TypeName##Pool; \
    \
    static inline void TypeName##_pool_init(TypeName##Pool *pool) { \
        pool->free_count = PoolSize; \
        for (int i = 0; i < PoolSize; i++) { \
            pool->free_list[i] = i; \
        } \
    } \
    \
    static inline ElementType* TypeName##_pool_alloc(TypeName##Pool *pool) { \
        if (pool->free_count == 0) return NULL; \
        int idx = pool->free_list[--pool->free_count]; \
        return &pool->elements[idx]; \
    } \
    \
    static inline void TypeName##_pool_free(TypeName##Pool *pool, ElementType *elem) { \
        int idx = elem - pool->elements; \
        pool->free_list[pool->free_count++] = idx; \
    }

// 使用
DEFINE_POOL_TYPE(Node, struct TreeNode, 1000)

NodePool pool;
Node_pool_init(&pool);
struct TreeNode *node = Node_pool_alloc(&pool);
Node_pool_free(&pool, node);
```

---

## 3. 内存对齐与布局

### 3.1 对齐技术

```c
#include <stdalign.h>
#include <immintrin.h>

// 编译器对齐
alignas(64) char buffer[1024];  // 64字节对齐（缓存行）

// 运行时对齐
void* aligned_malloc(size_t size, size_t alignment) {
    void *ptr = malloc(size + alignment + sizeof(void*));
    if (!ptr) return NULL;

    // 计算对齐地址
    void *aligned = (void *)(((uintptr_t)ptr + sizeof(void*) + alignment - 1)
                              & ~(alignment - 1));

    // 保存原始指针
    ((void**)aligned)[-1] = ptr;
    return aligned;
}

void aligned_free(void *ptr) {
    if (ptr) free(((void**)ptr)[-1]);
}

// SIMD对齐示例
void process_simd(float *input, float *output, int n) {
    // 确保16字节对齐（SSE）或32字节对齐（AVX）
    assert(((uintptr_t)input & 0xF) == 0);

    for (int i = 0; i < n; i += 8) {
        __m256 vec = _mm256_load_ps(&input[i]);  // 对齐加载
        vec = _mm256_sqrt_ps(vec);
        _mm256_store_ps(&output[i], vec);
    }
}
```

### 3.2 结构体内存布局

```c
// 糟糕的内存布局 (32字节)
struct BadLayout {
    char a;      // +0
    // 填充7字节
    double b;    // +8
    char c;      // +16
    // 填充7字节
    double d;    // +24
};  // 总计32字节

// 优化布局 (24字节)
struct GoodLayout {
    double b;    // +0
    double d;    // +8
    char a;      // +16
    char c;      // +17
    // 填充6字节
};  // 总计24字节

// 手动填充控制
#pragma pack(push, 1)
struct Packed {
    char a;
    double b;  // 可能跨越缓存行！性能差
};
#pragma pack(pop)
```

---

## 4. 内存屏障与排序

### 4.1 编译器屏障

```c
// 防止编译器重排序
#define COMPILER_BARRIER() __asm__ volatile ("" ::: "memory")

void example() {
    int a = 1;
    COMPILER_BARRIER();
    int b = a;  // 保证a=1在此执行
}
```

### 4.2 CPU内存屏障

```c
// x86内存屏障
#define MFENCE() __asm__ volatile ("mfence" ::: "memory")
#define SFENCE() __asm__ volatile ("sfence" ::: "memory")
#define LFENCE() __asm__ volatile ("lfence" ::: "memory")

// 标准C11接口
#include <stdatomic.h>

atomic_thread_fence(memory_order_acquire);  // 读屏障
atomic_thread_fence(memory_order_release);  // 写屏障
atomic_thread_fence(memory_order_seq_cst);  // 全屏障
```

---

## 5. 大页内存

### 5.1 使用大页

```c
#include <sys/mman.h>

// 分配2MB大页
void* alloc_hugepage(size_t size) {
    void *ptr = mmap(NULL, size,
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
                     -1, 0);
    if (ptr == MAP_FAILED) return NULL;
    return ptr;
}

// 透明大页(THP) - 自动
// /sys/kernel/mm/transparent_hugepage/enabled
// echo always > /sys/kernel/mm/transparent_hugepage/enabled
```

---

## 关联导航

### 前置知识

- [内存管理](02_Memory_Management.md)
- [内存对齐与布局](./README.md)

### 后续延伸

- [并发编程](../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md)
- [性能优化](../05_Engineering_Layer/03_Performance_Optimization.md)
