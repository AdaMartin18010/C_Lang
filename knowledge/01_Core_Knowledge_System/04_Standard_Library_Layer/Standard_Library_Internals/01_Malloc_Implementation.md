# malloc/free 实现原理深度解析

> **实现**: ptmalloc (glibc) / jemalloc / tcmalloc
> **平台**: Linux x86_64
> **源码**: <https://sourceware.org/glibc/wiki/MallocInternals>

---

## 1. 内存分配器架构

### 1.1 整体结构

```
程序请求 malloc(size)
    ↓
[快速路径] 线程缓存 (TCache) - 无锁分配
    ↓ (未命中)
[中速路径] 竞技场 (Arena) - 分区锁
    ↓ (未命中)
[慢速路径] 系统调用 (mmap/brk) - 全局锁
```

### 1.2 ptmalloc核心结构

```c
// 简化版ptmalloc结构
typedef struct malloc_chunk {
    size_t mchunk_prev_size;  // 前一个chunk大小 (如果前一个空闲)
    size_t mchunk_size;       // 当前chunk大小 + 标志位

    struct malloc_chunk* fd;  // 前向指针 (仅空闲)
    struct malloc_chunk* bk;  // 后向指针 (仅空闲)
} malloc_chunk;

// Chunk状态标志位 (低3位)
#define PREV_INUSE 0x1   // 前一个chunk在使用中
#define IS_MMAPPED 0x2   // 通过mmap分配
#define NON_MAIN_ARENA 0x4 // 非主arena
```

---

## 2. Chunk结构详解

### 2.1 已分配Chunk

```
已分配Chunk布局 (32位系统示例):
+------------------+
| prev_size (4)    | <- 仅当prev空闲时有意义
+------------------+
| size (4)         | <- 包含: 大小 | PREV_INUSE | IS_MMAPPED | NON_MAIN_ARENA
+------------------+
| user data (N)    | <- 返回给用户的指针指向这里
| ...              |
+------------------+
| 下一个chunk的prev_size | <- 复用为当前chunk的footer
+------------------+
```

### 2.2 空闲Chunk

```
空闲Chunk布局:
+------------------+
| prev_size        |
+------------------+
| size             |
+------------------+
| fd (前向指针)     | -> 下一个空闲chunk
+------------------+
| bk (后向指针)     | -> 前一个空闲chunk
+------------------+
| 空闲空间...       |
+------------------+
| size (副本)       | <- footer用于合并
+------------------+
```

---

## 3. 分配算法

### 3.1 小内存分配 (Fast Bin)

```c
// Fast bin: 小于一定大小的内存快速分配
// 大小: 16, 24, 32, ..., 80, 88, 96, ..., 128 字节

// Fast bin结构: 单向链表 (LIFO)
// 分配和释放都是O(1)，且不合并

void* fastbin_alloc(size_t size) {
    int idx = fastbin_index(size);

    // 检查线程缓存 (TCache)
    void* ptr = tcache_get(idx);
    if (ptr) return ptr;

    // 检查fast bin
    malloc_chunk* victim = fastbinsY[idx];
    if (victim) {
        fastbinsY[idx] = victim->fd;  // 移除链表头
        return chunk2mem(victim);
    }

    // 从其他来源分配...
}

// 释放 (延迟合并)
void fastbin_free(void* ptr) {
    malloc_chunk* p = mem2chunk(ptr);
    int idx = fastbin_index(chunksize(p));

    // 放入fast bin链表头
    p->fd = fastbinsY[idx];
    fastbinsY[idx] = p;
}
```

### 3.2 普通内存分配 (Small/Large Bin)

```c
// Small bin: 小于512字节 (32位) / 1024字节 (64位)
// Large bin: 更大尺寸，按大小排序

void* bin_alloc(size_t size) {
    // 1. 尝试从unsorted bin分配
    // 2. 尝试从small/large bin分配
    // 3. 切割top chunk
    // 4. 系统调用扩展堆

    // 最佳适配 vs 首次适配权衡
    malloc_chunk* victim = ...;

    // 如果chunk过大，分割它
    if (chunksize(victim) - size >= MINSIZE) {
        malloc_chunk* remainder = split_chunk(victim, size);
        // 将remainder放入unsorted bin
    }

    return chunk2mem(victim);
}
```

### 3.3 大内存分配 (mmap)

```c
// 大于阈值 (默认128KB) 使用mmap
void* large_alloc(size_t size) {
    size_t pagesize = sysconf(_SC_PAGESIZE);
    size_t pages = (size + pagesize - 1) / pagesize;

    void* ptr = mmap(NULL, pages * pagesize,
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS,
                     -1, 0);

    // 标记为mmap分配的chunk
    return ptr;
}

void large_free(void* ptr, size_t size) {
    munmap(ptr, size);
}
```

---

## 4. 释放与合并

### 4.1 合并空闲Chunk

```c
// 释放时合并相邻空闲chunk，减少碎片
void consolidate(void* ptr) {
    malloc_chunk* p = mem2chunk(ptr);
    size_t size = chunksize(p);

    // 检查前一个chunk是否空闲
    if (!prev_inuse(p)) {
        malloc_chunk* prev = prev_chunk(p);
        size += chunksize(prev);
        p = prev;
        unlink_chunk(p);  // 从bin中移除
    }

    // 检查后一个chunk是否空闲
    malloc_chunk* next = next_chunk(p);
    if (!inuse(next)) {
        size += chunksize(next);
        unlink_chunk(next);
    } else {
        clear_inuse_bit(next);
    }

    // 设置新大小
    set_head(p, size | PREV_INUSE);
    set_foot(p, size);

    // 放入unsorted bin
    unsorted_chunks_insert(p);
}
```

### 4.2 双向链表操作

```c
// 从bin中移除chunk (unlink)
#define unlink_chunk(P) do { \
    malloc_chunk* FD = (P)->fd; \
    malloc_chunk* BK = (P)->bk; \
    FD->bk = BK; \
    BK->fd = FD; \
} while(0)

// 插入bin
#define insert_chunk(P, B) do { \
    malloc_chunk* FD = (B)->fd; \
    (P)->fd = FD; \
    (P)->bk = (B); \
    FD->bk = (P); \
    (B)->fd = (P); \
} while(0)
```

---

## 5. 线程安全 (Arena)

### 5.1 多线程架构

```c
// 每个线程有自己的arena或共享arena
// 主arena: 通过brk扩展的数据段
// 非主arena: 通过mmap分配的独立区域

typedef struct arena {
    malloc_state* next;           // 下一个arena
    malloc_state* next_free;      // 空闲arena列表

    mutex_t mutex;                // 每个arena独立锁

    // 各种bin
    malloc_chunk* fastbinsY[NFASTBINS];
    malloc_chunk* bins[NBINS * 2 - 2];  // small/large bins

    malloc_chunk* top;            // top chunk
    size_t system_mem;            // 从系统分配的内存
} arena;

// 线程获取arena
arena* get_arena(void) {
    arena* a = thread_arena;
    if (a) return a;

    // 尝试找到空闲arena
    a = find_free_arena();
    if (a) {
        thread_arena = a;
        return a;
    }

    // 创建新arena
    a = create_new_arena();
    thread_arena = a;
    return a;
}
```

### 5.2 TCache (线程缓存)

```c
// 每个线程的本地缓存，无锁分配
typedef struct tcache_entry {
    struct tcache_entry* next;
} tcache_entry;

typedef struct tcache_perthread_struct {
    tcache_entry* entries[TCACHE_MAX_BINS];
    uint16_t counts[TCACHE_MAX_BINS];
} tcache;

__thread tcache* thread_tcache;

// 从tcache分配 - 无锁!
void* tcache_alloc(size_t size) {
    int idx = tcache_idx(size);
    tcache_entry* e = thread_tcache->entries[idx];

    if (e) {
        thread_tcache->entries[idx] = e->next;
        thread_tcache->counts[idx]--;
        return e;
    }

    // tcache为空，从arena填充
    return tcache_fill_and_alloc(idx);
}
```

---

## 6. 性能优化

### 6.1 内存对齐

```c
// malloc返回的内存总是对齐到2 * sizeof(size_t)
// 64位系统: 16字节对齐

void* aligned_malloc(size_t size, size_t alignment) {
    void* ptr = malloc(size + alignment - 1 + sizeof(void*));
    if (!ptr) return NULL;

    // 计算对齐地址
    void* aligned = (void*)(((uintptr_t)ptr + sizeof(void*) + alignment - 1)
                            & ~(alignment - 1));

    // 存储原始指针在aligned之前
    ((void**)aligned)[-1] = ptr;

    return aligned;
}

void aligned_free(void* ptr) {
    if (ptr) {
        void* original = ((void**)ptr)[-1];
        free(original);
    }
}
```

### 6.2 内存碎片处理

```c
// 定期合并fast bin
void malloc_consolidate(arena* ar_ptr) {
    // 遍历所有fast bin
    for (int i = 0; i < NFASTBINS; i++) {
        malloc_chunk* p = ar_ptr->fastbinsY[i];
        ar_ptr->fastbinsY[i] = NULL;

        while (p) {
            malloc_chunk* next = p->fd;
            consolidate(p);  // 合并相邻空闲chunk
            p = next;
        }
    }
}

// 内存归还系统 (trim)
int malloc_trim(size_t pad) {
    // 释放top chunk回系统
    // 解除mmap的大块内存
}
```

---

## 7. 调试与分析

### 7.1 调试环境变量

```bash
# 启用malloc调试
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
export MALLOC_CHECK_=3      # glibc malloc检查
export MALLOC_TRACE=output  # 记录malloc调用

# jemalloc统计
export MALLOC_CONF="stats_print:true"

# 检测内存错误
export LD_PRELOAD=/usr/lib/libasan.so  # AddressSanitizer
```

### 7.2 mallinfo查看统计

```c
#include <malloc.h>

void print_malloc_stats(void) {
    struct mallinfo2 mi = mallinfo2();

    printf("Total arena space: %zu\n", mi.arena);
    printf("Ordinary blocks: %zu\n", mi.ordblks);
    printf("Small blocks: %zu\n", mi.smblks);
    printf("Holding blocks: %zu\n", mi.hblks);
    printf("Total allocated: %zu\n", mi.uordblks);
    printf("Total free: %zu\n", mi.fordblks);
}

// 或输出到stderr
void malloc_stats(void) {
    malloc_stats();  // glibc函数
}
```

### 7.3 heap分析工具

```bash
# 使用gdb分析堆
gdb -p <pid>
(gdb) p main_arena
(gdb) x/100gx &main_arena

# 使用pwndbg/heaptrace
heap
bins
fastbins

# heaptrack (GUI分析)
heaptrack ./program
heaptrack_gui heaptrack.program.*.gz
```

---

## 8. 自定义内存分配器

### 8.1 简单池分配器

```c
typedef struct pool {
    char* buffer;
    size_t size;
    size_t used;
    struct pool* next;
} pool;

pool* pool_create(size_t size) {
    pool* p = malloc(sizeof(pool) + size);
    p->buffer = (char*)(p + 1);
    p->size = size;
    p->used = 0;
    p->next = NULL;
    return p;
}

void* pool_alloc(pool* p, size_t size) {
    size = (size + 7) & ~7;  // 8字节对齐

    if (p->used + size > p->size) {
        if (!p->next) {
            p->next = pool_create(p->size * 2);
        }
        return pool_alloc(p->next, size);
    }

    void* ptr = p->buffer + p->used;
    p->used += size;
    return ptr;
}

void pool_destroy(pool* p) {
    while (p) {
        pool* next = p->next;
        free(p);
        p = next;
    }
}
```

---

## 9. 参考

- **glibc malloc**: <https://sourceware.org/glibc/wiki/MallocInternals>
- **jemalloc**: <http://jemalloc.net/>
- **tcmalloc**: <https://google.github.io/tcmalloc/>
- **ptmalloc paper**: Wolfram Gloger's ptmalloc

---

**最后更新**: 2026-03-24
