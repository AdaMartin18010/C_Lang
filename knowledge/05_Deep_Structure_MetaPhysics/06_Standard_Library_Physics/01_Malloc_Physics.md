# malloc物理内存管理

> **层级定位**: 05 Deep Structure MetaPhysics / 06 Standard Library Physics
> **对应标准**: ptmalloc, jemalloc, tcmalloc, C11
> **难度级别**: L4 熟练
> **预估学习时间**: 12-15 小时

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | 内存分配器、碎片管理、bins、arena、延迟合并 |
| **前置知识** | 虚拟内存、系统调用、数据对齐 |
| **后续延伸** | 自定义分配器、内存池、NUMA感知分配 |
| **权威来源** | ptmalloc, jemalloc, tcmalloc, CS:APP |

---


---

## 📑 目录

- [malloc物理内存管理](#malloc物理内存管理)
  - [📋 本节概要](#-本节概要)
  - [📑 目录](#-目录)
  - [🧠 知识结构思维导图](#-知识结构思维导图)
  - [📖 核心概念详解](#-核心概念详解)
    - [1. 内存分配器架构](#1-内存分配器架构)
      - [1.1 ptmalloc架构](#11-ptmalloc架构)
      - [1.2 分配流程](#12-分配流程)
    - [2. 碎片管理](#2-碎片管理)
      - [2.1 内部碎片与外部碎片](#21-内部碎片与外部碎片)
      - [2.2 延迟合并策略](#22-延迟合并策略)
    - [3. 系统接口优化](#3-系统接口优化)
      - [3.1 brk vs mmap](#31-brk-vs-mmap)
      - [3.2 内存建议](#32-内存建议)
    - [4. 高级分配器](#4-高级分配器)
      - [4.1 jemalloc特性](#41-jemalloc特性)
      - [4.2 自定义分配器](#42-自定义分配器)
  - [⚠️ 常见陷阱](#️-常见陷阱)
    - [陷阱 MALLOC01: 内存泄漏检测遗漏](#陷阱-malloc01-内存泄漏检测遗漏)
    - [陷阱 MALLOC02: 双重释放](#陷阱-malloc02-双重释放)
    - [陷阱 MALLOC03: 使用已释放内存](#陷阱-malloc03-使用已释放内存)
  - [✅ 质量验收清单](#-质量验收清单)
  - [📚 参考资源](#-参考资源)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 🧠 知识结构思维导图

```mermaid
mindmap
  root((malloc物理))
    分配器架构
      Arena管理
      线程缓存
      全局堆
    块管理
      Chunk结构
      Bins索引
      延迟合并
    系统接口
      brk/mmap
      madvise
      hugepage
    优化策略
      大小分级
      对齐策略
      回收策略
```

---

## 📖 核心概念详解

### 1. 内存分配器架构

#### 1.1 ptmalloc架构

```c
/*
 * ptmalloc（glibc默认分配器）架构：
 *
 * 1. Arena：独立的内存分配区域
 *    - 主arena：使用brk()
 *    - 副arena：使用mmap()
 *    - 每线程一个arena（减少竞争）
 *
 * 2. Heap：arena内的内存块
 *    - 通过sub-heap管理
 *
 * 3. Chunk：实际分配的内存单元
 *    - 包含元数据（大小、标志位）
 *    - 相邻空闲块合并
 *
 * 4. Bins：空闲块分类存储
 *    - Fast bins：小块快速分配
 *    - Small bins：精确大小匹配
 *    - Large bins：范围匹配
 *    - Unsorted bin：合并缓冲区
 */

// ptmalloc核心数据结构（简化）

typedef struct malloc_chunk {
    size_t prev_size;      // 前一个块大小（如果前一个块空闲）
    size_t size;           // 当前块大小 + 标志位

    struct malloc_chunk *fd;  // 前向指针（空闲时）
    struct malloc_chunk *bk;  // 后向指针（空闲时）
} mchunk;

// 标志位
#define PREV_INUSE 0x1     // 前一个块是否使用
#define IS_MMAPPED 0x2     // 是否mmap分配
#define NON_MAIN_ARENA 0x4 // 是否非主arena

#define SIZE_BITS (PREV_INUSE | IS_MMAPPED | NON_MAIN_ARENA)
#define chunksize(p) ((p)->size & ~SIZE_BITS)

// Arena结构
typedef struct malloc_state {
    int mutex;                    // 锁

    // Fast bins
    mchunk *fastbinsY[NFASTBINS];

    // Base bins（包含small/large/unsorted）
    mchunk *bins[NBINS * 2 - 2];

    // Top chunk（ wilderness ）
    mchunk *top;

    // Last remainder
    mchunk *last_remainder;

    // 统计
    size_t max_system_mem;
    size_t system_mem;
} mstate;

// Fast bin索引计算（8字节对齐，大小/8 - 2）
#define fastbin_index(sz) \
    ((((unsigned int)(sz)) >> (SIZE_SZ == 8 ? 4 : 3)) - 2)

// Small bin索引（64位系统）
#define smallbin_index(sz) \
    ((SMALLBIN_WIDTH == 16 ? (((unsigned)(sz)) >> 4) \
                           : (((unsigned)(sz)) >> 3)) + SMALLBIN_CORRECTION)
```

#### 1.2 分配流程

```c
/*
 * malloc(size) 分配流程：
 *
 * 1. 检查fast bins（大小 <= 0x80）
 *    - 精确匹配，LIFO
 *
 * 2. 检查small bins（0x80 < 大小 <= 0x400）
 *    - 精确匹配，FIFO
 *
 * 3. 检查large bins（大小 > 0x400）
 *    - 最佳适配或首次适配
 *
 * 4. 检查unsorted bin
 *    - 遍历，可能触发合并
 *
 * 5. 从top chunk分配
 *    - 切割top chunk
 *
 * 6. 系统调用扩展内存
 *    - brk() 或 mmap()
 */

// 模拟malloc实现
void* my_malloc(size_t size) {
    // 对齐到8/16字节
    size_t aligned_size = ALIGN(size + SIZE_SZ, MALLOC_ALIGNMENT);

    // 1. 检查fast bins（小块）
    if (aligned_size <= MAX_FAST_SIZE) {
        int idx = fastbin_index(aligned_size);
        mchunk *chunk = arena->fastbinsY[idx];

        if (chunk != NULL) {
            // 从fast bin取出
            arena->fastbinsY[idx] = chunk->fd;

            // 标记为使用
            set_inuse(chunk, aligned_size);

            return chunk2mem(chunk);
        }
    }

    // 2. 检查small bins
    if (aligned_size < MIN_LARGE_SIZE) {
        int idx = smallbin_index(aligned_size);
        mchunk *victim = last(arena->bins, idx);

        if (victim != first(arena->bins, idx)) {
            // 找到精确匹配
            unlink_chunk(victim);
            set_inuse(victim, aligned_size);
            return chunk2mem(victim);
        }
    }

    // 3. 检查unsorted bin和large bins
    // ... 复杂逻辑

    // 4. 从top chunk分配
    mchunk *top = arena->top;
    size_t top_size = chunksize(top);

    if (top_size >= aligned_size + MINSIZE) {
        // 切割top chunk
        arena->top = chunk_at_offset(top, aligned_size);
        set_head(arena->top, top_size - aligned_size | PREV_INUSE);

        set_head(top, aligned_size | PREV_INUSE);
        return chunk2mem(top);
    }

    // 5. 系统调用扩展
    return sysmalloc(aligned_size, arena);
}

// 释放流程
void my_free(void *mem) {
    if (mem == NULL) return;

    mchunk *chunk = mem2chunk(mem);
    size_t size = chunksize(chunk);

    // 检查是否mmap分配
    if (chunk_is_mmapped(chunk)) {
        munmap_chunk(chunk);
        return;
    }

    // 检查是否可以放入fast bins
    if (size <= MAX_FAST_SIZE && get_fastchunks(arena)) {
        // 放入fast bin（LIFO）
        int idx = fastbin_index(size);
        chunk->fd = arena->fastbinsY[idx];
        arena->fastbinsY[idx] = chunk;
        return;
    }

    // 尝试合并相邻空闲块
    mchunk *next = chunk_at_offset(chunk, size);

    if (!inuse(next)) {
        // 合并后一个块
        size += chunksize(next);
        unlink_chunk(next);
    }

    if (!prev_inuse(chunk)) {
        // 合并前一个块
        size_t prev_size = chunk->prev_size;
        mchunk *prev = chunk_at_offset(chunk, -((long)prev_size));
        size += prev_size;
        chunk = prev;
        unlink_chunk(prev);
    }

    // 设置合并后的大小
    set_head(chunk, size | PREV_INUSE);
    set_foot(chunk, size);

    // 放入unsorted bin
    if (chunk != arena->top) {
        insert_unsorted(chunk);
    }
}
```

### 2. 碎片管理

#### 2.1 内部碎片与外部碎片

```c
/*
 * 碎片类型：
 *
 * 内部碎片：分配的块大于请求大小
 * - 对齐要求导致
 * - 元数据开销
 *
 * 外部碎片：空闲内存分散，无法满足大块请求
 * - 频繁分配释放不同大小块
 * - 需要合并策略
 */

// 内部碎片计算
size_t internal_fragmentation(void *ptr) {
    mchunk *chunk = mem2chunk(ptr);
    size_t allocated = chunksize(chunk);
    size_t requested = allocated - SIZE_SZ;  // 减去元数据

    return allocated - requested;
}

// 外部碎片度量
double external_fragmentation_ratio(Heap *heap) {
    size_t total_free = 0;
    size_t largest_free = 0;

    // 遍历所有空闲块
    for (mchunk *p = heap->free_list; p != NULL; p = p->fd) {
        size_t sz = chunksize(p);
        total_free += sz;
        if (sz > largest_free) {
            largest_free = sz;
        }
    }

    if (total_free == 0) return 0.0;

    // 碎片率 = 1 - (最大空闲块 / 总空闲内存)
    return 1.0 - ((double)largest_free / total_free);
}

// 减少碎片的分配策略
void* malloc_low_fragmentation(size_t size) {
    // 使用大小分级分配
    // 将请求向上取整到标准大小

    static const size_t size_classes[] = {
        8, 16, 24, 32, 48, 64, 96, 128,
        192, 256, 384, 512, 768, 1024,
        1536, 2048, 3072, 4096
    };

    // 找到合适的大小类
    size_t alloc_size = size;
    for (int i = 0; i < sizeof(size_classes)/sizeof(size_classes[0]); i++) {
        if (size_classes[i] >= size + SIZE_SZ) {
            alloc_size = size_classes[i];
            break;
        }
    }

    return my_malloc(alloc_size);
}
```

#### 2.2 延迟合并策略

```c
/*
 * ptmalloc的延迟合并策略：
 *
 * 1. Fast bins：不合并，快速分配释放
 * 2. 小释放：可能放入unsorted bin，延迟合并
 * 3. 大分配前：合并unsorted bin中的块
 * 4. 内存紧缩：malloc_trim()时完全合并
 */

// 触发合并的时机
void malloc_consolidate(mstate *av) {
    // 合并fast bins到普通bins
    for (int i = 0; i < NFASTBINS; i++) {
        mchunk *p = av->fastbinsY[i];

        while (p != NULL) {
            mchunk *next = p->fd;
            size_t size = chunksize(p);

            // 合并后一个块
            mchunk *nextchunk = chunk_at_offset(p, size);

            if (!inuse(nextchunk)) {
                size += chunksize(nextchunk);
                unlink_chunk(nextchunk);
            }

            // 合并前一个块
            if (!prev_inuse(p)) {
                size_t prevsize = p->prev_size;
                mchunk *prev = chunk_at_offset(p, -((long)prevsize));
                size += prevsize;
                p = prev;
                unlink_chunk(prev);
            }

            // 放入unsorted bin
            set_head(p, size | PREV_INUSE);
            set_foot(p, size);

            if (size >= MIN_LARGE_SIZE) {
                // 大 Chunk 可能需要放入 large bins
                insert_large_bin(av, p, size);
            } else {
                insert_unsorted(av, p);
            }

            p = next;
        }

        av->fastbinsY[i] = NULL;
    }
}

// 内存紧缩：归还内存给系统
int malloc_trim(size_t pad) {
    mstate *av = &main_arena;

    // 尝试收缩top chunk
    size_t top_size = chunksize(av->top);

    // 保留 pad 大小的缓冲
    if (top_size > pad + MINSIZE) {
        size_t release_size = top_size - pad;

        // 使用brk收缩或munmap释放
        if (av == &main_arena) {
            // 尝试收缩brk
            sbrk(-release_size);
        }
    }

    // 释放mmap的块
    // ...

    return 0;
}
```

### 3. 系统接口优化

#### 3.1 brk vs mmap

```c
/*
 * 内存获取策略：
 *
 * brk()：
 * - 优点：连续地址空间，低TLB压力
 * - 缺点：不能独立释放中间块
 * - 适用：小分配，长期持有的内存
 *
 * mmap()：
 * - 优点：独立释放，可归还系统
 * - 缺点：更高的开销，更多TLB条目
 * - 适用：大分配，短期使用的内存
 */

// ptmalloc的阈值策略
#define DEFAULT_MMAP_THRESHOLD (128 * 1024)  // 128KB
#define DEFAULT_MMAP_THRESHOLD_MAX (512 * 1024)

void* sysmalloc(size_t nb, mstate *av) {
    // 检查是否应该使用mmap
    if (av == NULL ||
        (nb > mp_.mmap_threshold && mp_.n_mmaps < mp_.n_mmaps_max)) {

        // 使用mmap
        size_t size = ALIGN_UP(nb + SIZE_SZ, pagesize);
        char *mm = (char*)mmap(NULL, size, PROT_READ | PROT_WRITE,
                               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        if (mm != MAP_FAILED) {
            // 设置mmap标志
            set_head((mchunk*)mm, size | IS_MMAPPED);

            mp_.n_mmaps++;
            mp_.mmapped_mem += size;

            return chunk2mem((mchunk*)mm);
        }
    }

    // 使用brk扩展堆
    // ...
}

// 动态调整mmap阈值
void adjust_mmap_threshold(size_t released_size) {
    // 如果释放了大块，降低阈值以更多使用mmap
    if (released_size > DEFAULT_MMAP_THRESHOLD) {
        mp_.mmap_threshold =
            (mp_.mmap_threshold + released_size) / 2;

        // 限制上限
        if (mp_.mmap_threshold > DEFAULT_MMAP_THRESHOLD_MAX) {
            mp_.mmap_threshold = DEFAULT_MMAP_THRESHOLD_MAX;
        }
    }
}
```

#### 3.2 内存建议

```c
// madvise优化
#include <sys/mman.h>

// 释放内存给系统（但不unmap）
void malloc_madvise_free(void *ptr, size_t size) {
    // 告诉内核可以回收这些页面
    madvise(ptr, size, MADV_FREE);

    // 页面内容保留，但可被回收
    // 再次访问时，页面会重新分配（内容为0或原值，未定义）
}

// 顺序访问提示
void malloc_madvise_sequential(void *ptr, size_t size) {
    // 提示内核这些页面将顺序访问
    madvise(ptr, size, MADV_SEQUENTIAL);

    // 内核可能预取并减少缓存保持
}

// 随机访问提示
void malloc_madvise_random(void *ptr, size_t size) {
    madvise(ptr, size, MADV_RANDOM);

    // 内核禁用预取
}

// 巨大页支持
void* malloc_hugepage(size_t size) {
    size_t hugepage_size = 2 * 1024 * 1024;  // 2MB

    // 对齐到巨大页边界
    size = ALIGN_UP(size, hugepage_size);

    void *mem = mmap(NULL, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
                     -1, 0);

    if (mem == MAP_FAILED) {
        // 回退到透明巨大页
        mem = mmap(NULL, size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        madvise(mem, size, MADV_HUGEPAGE);
    }

    return mem;
}
```

### 4. 高级分配器

#### 4.1 jemalloc特性

```c
/*
 * jemalloc设计特点：
 *
 * 1. 大小分级（size classes）：减少碎片
 * 2. 线程缓存（tcache）：减少锁竞争
 * 3. Arena扩展：更好的并发支持
 * 4. 延迟合并：减少合并开销
 */

typedef struct {
    // 大小类（small/large/huge）
    size_t small_min;
    size_t small_max;
    size_t large_max;

    // 线程缓存
    tcache_t *tcache;

    // Arena数组
    arena_t **arenas;
    unsigned narenas;
} jemalloc_ctl;

// jemalloc分配流程
void* je_malloc(size_t size) {
    // 1. 检查线程缓存
    if (size <= SMALL_MAXCLASS) {
        void *ptr = tcache_alloc_small(tcache, size);
        if (ptr != NULL) return ptr;
    }

    // 2. 检查arena
    arena_t *arena = choose_arena();

    if (size <= SMALL_MAXCLASS) {
        return arena_malloc_small(arena, size);
    } else if (size <= large_maxclass) {
        return arena_malloc_large(arena, size);
    } else {
        return huge_malloc(size);
    }
}

// jemalloc的extent管理
typedef struct extent_s {
    // 红黑树节点
    rb_node(extent_s) rb_link;

    // 地址和大小
    void *addr;
    size_t size;

    // 状态
    bool slab;      // 是否用于小分配
    unsigned binind; // 大小类索引
} extent_t;
```

#### 4.2 自定义分配器

```c
// 内存池分配器（固定大小对象）
typedef struct {
    void *pool;           // 内存池基址
    size_t object_size;   // 对象大小
    size_t pool_size;     // 池总大小

    // 空闲列表（单链表）
    void *free_list;

    // 当前分配位置（简单 bump allocator）
    void *current;
    void *end;
} PoolAllocator;

PoolAllocator* pool_create(size_t object_size, size_t num_objects) {
    PoolAllocator *pool = malloc(sizeof(PoolAllocator));

    pool->object_size = ALIGN_UP(object_size, sizeof(void*));
    pool->pool_size = pool->object_size * num_objects;
    pool->pool = mmap(NULL, pool->pool_size,
                      PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // 初始化空闲列表
    pool->free_list = NULL;
    pool->current = pool->pool;
    pool->end = (char*)pool->pool + pool->pool_size;

    return pool;
}

void* pool_alloc(PoolAllocator *pool) {
    // 优先从空闲列表分配
    if (pool->free_list != NULL) {
        void *ptr = pool->free_list;
        pool->free_list = *(void**)ptr;  // 下一个空闲
        return ptr;
    }

    // bump分配
    if ((char*)pool->current + pool->object_size <= (char*)pool->end) {
        void *ptr = pool->current;
        pool->current = (char*)pool->current + pool->object_size;
        return ptr;
    }

    return NULL;  // 池耗尽
}

void pool_free(PoolAllocator *pool, void *ptr) {
    // 归还到空闲列表
    *(void**)ptr = pool->free_list;
    pool->free_list = ptr;
}
```

---

## ⚠️ 常见陷阱

### 陷阱 MALLOC01: 内存泄漏检测遗漏

```c
// 错误：未匹配分配和释放
void wrong_allocation_pattern(void) {
    char *buf = malloc(100);
    if (condition) {
        return;  // ❌ 泄漏！
    }
    free(buf);
}

// 正确：使用RAII或确保释放
void correct_allocation_pattern(void) {
    char *buf = malloc(100);
    if (!buf) return;

    if (condition) {
        free(buf);  // ✅
        return;
    }

    free(buf);
}

// 更好的方式：使用__attribute__((cleanup))
void auto_free(void *p) { free(*(void**)p); }

#define AUTO __attribute__((cleanup(auto_free)))

void improved_pattern(void) {
    AUTO char *buf = malloc(100);
    if (!buf) return;

    // buf会自动释放
}
```

### 陷阱 MALLOC02: 双重释放

```c
// 错误：双重释放
void double_free_bug(void) {
    char *buf = malloc(100);
    free(buf);
    // ... 其他代码 ...
    free(buf);  // ❌ 双重释放！
}

// 正确：释放后置空
void correct_free(void) {
    char *buf = malloc(100);
    free(buf);
    buf = NULL;  // ✅

    // free(NULL) 是安全的
    free(buf);  // 无操作
}
```

### 陷阱 MALLOC03: 使用已释放内存

```c
// 错误：使用已释放内存（use-after-free）
void use_after_free_bug(void) {
    char *buf = malloc(100);
    strcpy(buf, "hello");
    free(buf);

    printf("%s\n", buf);  // ❌ 未定义行为！
}

// 检测方法：使用调试分配器
#ifdef DEBUG
void* debug_malloc(size_t size, const char *file, int line) {
    void *ptr = malloc(size + 8);
    // 添加魔数/边界检测
    *(size_t*)((char*)ptr + size) = MAGIC;
    // 记录分配位置
    record_allocation(ptr, size, file, line);
    return ptr;
}

void debug_free(void *ptr) {
    // 检查魔数
    // 标记为释放但未真正释放
    mark_freed(ptr);
}
#endif
```

---

## ✅ 质量验收清单

- [x] ptmalloc架构理解
- [x] chunk结构和元数据
- [x] bins分类（fast/small/large/unsorted）
- [x] 分配和释放流程
- [x] 内部/外部碎片
- [x] 延迟合并策略
- [x] brk vs mmap选择
- [x] madvise优化
- [x] 内存池实现
- [x] Mermaid思维导图
- [x] 常见陷阱与解决方案

---

## 📚 参考资源

| 资源 | 作者/来源 | 说明 |
|:-----|:----------|:-----|
| ptmalloc | Wolfram Gloger | glibc分配器 |
| jemalloc | Jason Evans | 现代分配器 |
| tcmalloc | Google | 高性能分配器 |
| CS:APP | Bryant & O'Hallaron | 内存章节 |

---

> **更新记录**
>
> - 2025-03-09: 初版创建，包含malloc物理内存管理完整分析


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
