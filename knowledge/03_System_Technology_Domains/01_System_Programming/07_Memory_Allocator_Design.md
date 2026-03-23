# 内存分配器设计原理

> **层级定位**: 03 System Technology Domains / 01 System Programming
> **难度级别**: L4-L5
> **预估学习时间**: 10-12 小时

---

## 1. 内存分配器概述

### 1.1 为什么需要自定义分配器

```
标准malloc的问题：
- 通用设计，非针对特定场景优化
- 线程安全开销（锁竞争）
- 碎片累积
- 不可预测的延迟
- 元数据开销

自定义分配器适用场景：
✓ 游戏引擎（帧分配器）
✓ 高频交易（零延迟要求）
✓ 嵌入式系统（确定性内存）
✓ 网络服务器（减少锁竞争）
✓ GPU计算（统一内存管理）
```

### 1.2 分配器类型对比

| 分配器类型 | 分配复杂度 | 释放复杂度 | 碎片 | 适用场景 |
|:-----------|:-----------|:-----------|:-----|:---------|
| **线性分配器** | O(1) | 批量O(1) | 无 | 帧临时数据 |
| **栈分配器** | O(1) | O(1) | 无 | LIFO模式 |
| **池分配器** | O(1) | O(1) | 无 | 固定大小对象 |
| **自由列表** | O(1) | O(1) | 中 | 小对象分配 |
| **伙伴系统** | O(log n) | O(log n) | 低 | 内核内存管理 |
| **slab分配器** | O(1) | O(1) | 无 | 内核对象缓存 |

---

## 2. 线性分配器（Arena）

### 2.1 原理

```
Arena分配器：

初始状态：        分配A(100B)后：     分配B(50B)后：
┌─────────┐       ┌─────────┐         ┌─────────┐
│         │       │ AAAAAAAA│         │ AAAAAAAA│
│         │       │ AAAAAAAA│         │ AAAAAAAA│
│         │       │         │         │ BBBBBBBB│
│         │       │         │         │ BBBBB   │
│ <-base  │       │ <-base  │         │ <-base  │
│  offset→│       │  offset→│         │  offset→│
└─────────┘       └─────────┘         └─────────┘

批量释放：重置offset即可
```

### 2.2 实现

```c
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

typedef struct {
    uint8_t *buffer;
    size_t capacity;
    size_t offset;
    size_t prev_offset;  // 用于对齐回退
} arena_t;

void arena_init(arena_t *a, void *buffer, size_t capacity) {
    a->buffer = buffer;
    a->capacity = capacity;
    a->offset = 0;
    a->prev_offset = 0;
}

void *arena_alloc(arena_t *a, size_t size, size_t align) {
    // 计算对齐地址
    uintptr_t ptr = (uintptr_t)(a->buffer + a->offset);
    uintptr_t aligned = (ptr + align - 1) & ~(align - 1);
    size_t padding = aligned - ptr;
    
    if (a->offset + padding + size > a->capacity) {
        return NULL;  // OOM
    }
    
    a->prev_offset = a->offset;
    a->offset += padding + size;
    
    return (void *)aligned;
}

void arena_free_all(arena_t *a) {
    a->offset = 0;
    a->prev_offset = 0;
}

// 示例用法
void example(void) {
    static uint8_t buffer[1024 * 1024];  // 1MB arena
    arena_t arena;
    arena_init(&arena, buffer, sizeof(buffer));
    
    // 游戏帧分配
    for (int frame = 0; frame < 1000; frame++) {
        // 分配临时数据
        void *temp1 = arena_alloc(&arena, 1024, 8);
        void *temp2 = arena_alloc(&arena, 2048, 16);
        
        // 使用...
        
        // 帧结束，批量释放
        arena_free_all(&arena);
    }
}
```

---

## 3. 池分配器

### 3.1 原理

```
池分配器（固定大小）：

空闲列表：        分配后：           释放后：
┌─────┐          ┌─────┐           ┌─────┐
│ 0   │────┐     │ 1   │────┐      │ 0   │────┐
└─────┘    │     └─────┘    │      └─────┘    │
           ▼                ▼                 ▼
┌─────┐   ┌─────┐   ┌─────┐   ┌─────┐   ┌─────┐
│next │   │next │──→│next │   │     │   │next │──→...
│ 1   │   │ 2   │   │NULL │   │USED │   │ 2   │
└─────┘   └─────┘   └─────┘   └─────┘   └─────┘
  ↑                                      ↑
空闲头部                               释放的块回到头部
```

### 3.2 实现

```c
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

typedef struct pool_block {
    struct pool_block *next;
} pool_block_t;

typedef struct {
    pool_block_t *free_list;
    uint8_t *buffer;
    size_t block_size;
    size_t block_count;
    size_t used;
} pool_t;

void pool_init(pool_t *p, void *buffer, size_t block_size, size_t block_count) {
    // 确保块大小至少能存指针
    if (block_size < sizeof(pool_block_t)) {
        block_size = sizeof(pool_block_t);
    }
    
    p->buffer = buffer;
    p->block_size = block_size;
    p->block_count = block_count;
    p->used = 0;
    
    // 初始化空闲列表
    p->free_list = NULL;
    for (size_t i = 0; i < block_count; i++) {
        pool_block_t *block = (pool_block_t *)((uint8_t *)buffer + i * block_size);
        block->next = p->free_list;
        p->free_list = block;
    }
}

void *pool_alloc(pool_t *p) {
    if (p->free_list == NULL) {
        return NULL;  // 池耗尽
    }
    
    pool_block_t *block = p->free_list;
    p->free_list = block->next;
    p->used++;
    
    return block;
}

void pool_free(pool_t *p, void *ptr) {
    if (ptr == NULL) return;
    
    // 验证ptr在池范围内（调试构建）
    #ifdef DEBUG
    uintptr_t buf_start = (uintptr_t)p->buffer;
    uintptr_t buf_end = buf_start + p->block_size * p->block_count;
    uintptr_t ptr_val = (uintptr_t)ptr;
    assert(ptr_val >= buf_start && ptr_val < buf_end);
    assert((ptr_val - buf_start) % p->block_size == 0);
    #endif
    
    pool_block_t *block = ptr;
    block->next = p->free_list;
    p->free_list = block;
    p->used--;
}
```

---

## 4. 栈分配器

### 4.1 原理与实现

```c
typedef struct {
    uint8_t *buffer;
    size_t capacity;
    size_t offset;
    struct {
        size_t offset;
        size_t prev_offset;
    } *marker_stack;
    size_t marker_count;
    size_t marker_capacity;
} stack_allocator_t;

void *stack_alloc(stack_allocator_t *sa, size_t size, size_t align) {
    // 对齐分配...
    // 同arena_alloc
    return arena_alloc((arena_t *)sa, size, align);
}

// 标记当前位置
size_t stack_marker(stack_allocator_t *sa) {
    assert(sa->marker_count < sa->marker_capacity);
    sa->marker_stack[sa->marker_count].offset = sa->offset;
    sa->marker_stack[sa->marker_count].prev_offset = sa->prev_offset;
    return sa->marker_count++;
}

// 释放到标记
void stack_free_to_marker(stack_allocator_t *sa, size_t marker) {
    assert(marker < sa->marker_count);
    sa->offset = sa->marker_stack[marker].offset;
    sa->prev_offset = sa->marker_stack[marker].prev_offset;
    sa->marker_count = marker;
}

// 使用示例
void stack_example(void) {
    // 作用域式分配
    size_t m1 = stack_marker(&sa);
    void *a = stack_alloc(&sa, 100, 8);
    {
        size_t m2 = stack_alloc(&sa, 200, 8);
        void *b = stack_alloc(&sa, 200, 8);
        // b在这里使用
        stack_free_to_marker(&sa, m2);  // 释放b
    }
    // a继续使用
    stack_free_to_marker(&sa, m1);  // 释放a
}
```

---

## 5. 多线程分配器

### 5.1 线程本地存储(TLS)分配器

```c
#include <threads.h>
#include <stdlib.h>

typedef struct {
    arena_t small_arena;
    pool_t small_pools[8];  // 不同大小的池
} tls_allocator_t;

_Thread_local tls_allocator_t *tls_alloc = NULL;

void tls_init(void) {
    if (tls_alloc == NULL) {
        tls_alloc = calloc(1, sizeof(tls_allocator_t));
        // 初始化各个arena和pool...
    }
}

void *malloc_fast(size_t size) {
    tls_init();
    
    if (size <= 64) {
        // 小对象使用池
        int pool_idx = size_to_pool_idx(size);
        return pool_alloc(&tls_alloc->small_pools[pool_idx]);
    } else if (size <= 4096) {
        // 中等对象使用线程本地arena
        return arena_alloc(&tls_alloc->small_arena, size, 16);
    } else {
        // 大对象使用系统malloc
        return malloc(size);
    }
}
```

### 5.2 无锁自由列表

```c
#include <stdatomic.h>

typedef _Atomic(struct lockfree_node *) lockfree_head_t;

typedef struct lockfree_node {
    struct lockfree_node *next;
} lockfree_node_t;

// Treiber栈 - 无锁LIFO
void *lockfree_pop(lockfree_head_t *head) {
    lockfree_node_t *old_head, *new_head;
    
    do {
        old_head = atomic_load(head);
        if (old_head == NULL) return NULL;
        new_head = old_head->next;
    } while (!atomic_compare_exchange_weak(head, &old_head, new_head));
    
    return old_head;
}

void lockfree_push(lockfree_head_t *head, void *ptr) {
    lockfree_node_t *new_head = ptr;
    lockfree_node_t *old_head;
    
    do {
        old_head = atomic_load(head);
        new_head->next = old_head;
    } while (!atomic_compare_exchange_weak(head, &old_head, new_head));
}
```

---

## 6. 调试与工具

### 6.1 分配跟踪

```c
#ifdef DEBUG_ALLOCATOR
    #define ALLOC_TRACE(fmt, ...) fprintf(stderr, "[ALLOC] " fmt "\n", ##__VA_ARGS__)
    
    typedef struct alloc_header {
        size_t size;
        const char *file;
        int line;
        struct alloc_header *next;
        uint32_t magic;
    } alloc_header_t;
    
    #define MAGIC_NUMBER 0xDEADBEEF
    
    void *debug_alloc(size_t size, const char *file, int line) {
        alloc_header_t *h = raw_alloc(sizeof(alloc_header_t) + size);
        h->size = size;
        h->file = file;
        h->line = line;
        h->magic = MAGIC_NUMBER;
        // 添加到全局列表...
        
        // 填充守卫区域
        memset((char *)h + sizeof(alloc_header_t) + size, 0xAA, 8);
        
        return (char *)h + sizeof(alloc_header_t);
    }
    
    #define ALLOC(size) debug_alloc(size, __FILE__, __LINE__)
#else
    #define ALLOC(size) raw_alloc(size)
#endif
```

---

## 关联导航

### 前置知识
- [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/01_Memory_Model.md)
- [内存对齐](../../../01_Core_Knowledge_System/02_Core_Layer/06_Memory_Alignment.md)
- [jemalloc/tcmalloc实现](../08_Memory_Management/02_Advanced_Allocators.md)

### 后续延伸
- [垃圾回收](./08_Garbage_Collection.md)
- [内存池在嵌入式中的应用](../../../03_Embedded_Systems/04_Memory_Optimization.md)

### 参考
- jemalloc: http://jemalloc.net/
- mimalloc: https://microsoft.github.io/mimalloc/
