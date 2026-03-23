# Lab 6: Malloc Lab - 动态内存分配器实验

## 1. 实验目标

### 1.1 总体目标

Malloc Lab通过实现一个动态内存分配器来深入理解内存管理和堆数据结构。

| 目标 | 描述 |
|------|------|
| **堆管理** | 理解堆的组织和管理 |
| **空闲链表** | 实现空闲块管理 |
| **分配策略** | 首次适配、下次适配、最佳适配 |
| **性能优化** | 提高内存利用率和吞吐量 |

### 1.2 实验要求

```
实现以下函数:
┌─────────────────────────────────────────┐
│ mm_init(void)        - 初始化分配器     │
│ mm_malloc(size_t)    - 分配内存块       │
│ mm_free(void*)       - 释放内存块       │
│ mm_realloc(void*)    - 调整内存块大小   │
└─────────────────────────────────────────┘

约束条件:
- 不能使用标准库malloc/free
- 使用mem_sbrk扩展堆
- 16字节对齐
- 最小块大小16字节
```

### 1.3 评分标准

| 指标 | 权重 | 目标 |
|------|------|------|
| 吞吐量 | 60% | 尽可能快 |
| 内存利用率 | 40% | 至少达到参考实现的80% |

---

## 2. 背景知识

### 2.1 堆组织

```
┌─────────────────────────────────────────┐
│            堆内存布局                   │
├─────────────────────────────────────────┤
│ ┌─────────┐ ← 堆起始 (mem_heap_lo())   │
│ │ 序言块  │  8字节，标记堆开始          │
│ ├─────────┤                            │
│ │ 块1     │  已分配或空闲               │
│ ├─────────┤                            │
│ │ 块2     │                            │
│ ├─────────┤                            │
│ │   ...   │                            │
│ ├─────────┤                            │
│ │ 结尾块  │  0大小，标记堆结束          │
│ └─────────┘ ← 堆结束 (mem_heap_hi())   │
└─────────────────────────────────────────┘
```

### 2.2 块结构

```c
// 隐式空闲链表块结构
┌─────────────────────────────────────────┐
│  块头部 (4字节)                         │
│  ┌─────────────┬──────────┬─────────┐  │
│  │ 块大小(29位) │ 分配位(1) │ 前一个 │  │
│  │              │          │ 分配位(1) │  │
│  └─────────────┴──────────┴─────────┘  │
├─────────────────────────────────────────┤
│  有效载荷 (对齐到8/16字节)              │
├─────────────────────────────────────────┤
│  填充 (可选)                            │
├─────────────────────────────────────────┤
│  块脚部 (4字节) - 仅在空闲块中存在      │
│  │  与头部相同的内容（便于合并）        │
└─────────────────────────────────────────┘
```

### 2.3 分配策略

```c
// 首次适配 (First Fit)
// 从头到尾扫描，找到第一个足够大的空闲块
// 优点：简单，快速
// 缺点：可能产生碎片

// 下次适配 (Next Fit)
// 从上次查找位置继续扫描
// 优点：分散碎片
// 缺点：可能错过前面的合适块

// 最佳适配 (Best Fit)
// 扫描所有块，找到最小的足够大的块
// 优点：减少内部碎片
// 缺点：扫描开销大
```

---

## 3. 隐式空闲链表实现

### 3.1 宏定义

```c
// mm.c
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* 基本常量 */
#define WSIZE       4       // 字大小（字节）
#define DSIZE       8       // 双字大小（字节）
#define CHUNKSIZE  (1<<12)  // 扩展堆的默认大小（4096字节）
#define ALIGNMENT   8       // 对齐要求

#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* 将大小和分配位打包到字中 */
#define PACK(size, alloc)  ((size) | (alloc))

/* 读写一个字 */
#define GET(p)       (*(unsigned int *)(p))
#define PUT(p, val)  (*(unsigned int *)(p) = (val))

/* 从地址p读取大小和分配位 */
#define GET_SIZE(p)   (GET(p) & ~0x7)
#define GET_ALLOC(p)  (GET(p) & 0x1)

/* 给定块指针bp，计算头部和脚部的地址 */
#define HDRP(bp)       ((char *)(bp) - WSIZE)
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* 给定块指针bp，计算下一个和前一个块的地址 */
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

/* 全局变量 */
static char *heap_listp = 0;  // 指向序言块
```

### 3.2 初始化

```c
/*
 * mm_init - 初始化malloc包
 * 创建带空闲块的堆
 */
int mm_init(void) {
    /* 创建初始空堆 */
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
        return -1;

    PUT(heap_listp, 0);                           // 对齐填充
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1)); // 序言块头部
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1)); // 序言块脚部
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));     // 结尾块头部
    heap_listp += (2 * WSIZE);

    /* 用CHUNKSIZE字节扩展空堆 */
    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;

    return 0;
}

/*
 * extend_heap - 扩展堆
 */
static void *extend_heap(size_t words) {
    char *bp;
    size_t size;

    /* 分配偶数个字以保持对齐 */
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;

    /* 初始化空闲块头部/脚部和结尾块头部 */
    PUT(HDRP(bp), PACK(size, 0));         // 空闲块头部
    PUT(FTRP(bp), PACK(size, 0));         // 空闲块脚部
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); // 新结尾块头部

    /* 如果前一个块空闲，则合并 */
    return coalesce(bp);
}
```

### 3.3 合并空闲块

```c
/*
 * coalesce - 边界标记合并
 * 合并bp周围的空闲块
 */
static void *coalesce(void *bp) {
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    /* 情况1: 前后都是已分配的 */
    if (prev_alloc && next_alloc) {
        return bp;
    }

    /* 情况2: 后面是空闲的 */
    else if (prev_alloc && !next_alloc) {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }

    /* 情况3: 前面是空闲的 */
    else if (!prev_alloc && next_alloc) {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }

    /* 情况4: 前后都是空闲的 */
    else {
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
                GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }

    return bp;
}
```

### 3.4 分配与释放

```c
/*
 * mm_malloc - 分配大小至少为size字节的块
 */
void *mm_malloc(size_t size) {
    size_t asize;      // 调整后的块大小
    size_t extendsize; // 如果没有合适块则扩展堆
    char *bp;

    /* 忽略虚假请求 */
    if (size == 0)
        return NULL;

    /* 调整为对齐要求 */
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

    /* 搜索空闲块 */
    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }

    /* 没有找到合适的块，获取更多内存 */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
        return NULL;
    place(bp, asize);
    return bp;
}

/*
 * mm_free - 释放块
 */
void mm_free(void *bp) {
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}

/*
 * find_fit - 首次适配搜索
 */
static void *find_fit(size_t asize) {
    void *bp;

    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))) {
            return bp;
        }
    }
    return NULL;  // 没有找到合适的块
}

/*
 * place - 将块放在空闲块的开始处，如果剩余部分足够大则分割
 */
static void place(void *bp, size_t asize) {
    size_t csize = GET_SIZE(HDRP(bp));

    if ((csize - asize) >= (2 * DSIZE)) {
        // 分割块
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));
    } else {
        // 不分割
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}
```

### 3.5 重新分配

```c
/*
 * mm_realloc - 重新分配块
 */
void *mm_realloc(void *ptr, size_t size) {
    size_t oldsize;
    void *newptr;

    /* 如果ptr为NULL，等价于malloc */
    if (ptr == NULL) {
        return mm_malloc(size);
    }

    /* 如果size为0，等价于free */
    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }

    /* 否则，分配新块并复制数据 */
    newptr = mm_malloc(size);
    if (newptr == NULL)
        return NULL;

    /* 复制旧数据 */
    oldsize = GET_SIZE(HDRP(ptr));
    if (size < oldsize)
        oldsize = size;
    memcpy(newptr, ptr, oldsize);

    /* 释放旧块 */
    mm_free(ptr);

    return newptr;
}
```

---

## 4. 显式空闲链表实现

### 4.1 块结构修改

```c
// 显式空闲链表块结构
┌─────────────────────────────────────────┐
│  块头部 (4字节)                         │
├─────────────────────────────────────────┤
│  前驱指针 (4/8字节) - 仅在空闲块中      │
├─────────────────────────────────────────┤
│  后继指针 (4/8字节) - 仅在空闲块中      │
├─────────────────────────────────────────┤
│  有效载荷 / 填充                        │
├─────────────────────────────────────────┤
│  块脚部 (4字节) - 仅在空闲块中          │
└─────────────────────────────────────────┘
```

### 4.2 显式链表操作

```c
// 额外宏定义
#define GET_PTR(p)      (*(unsigned long *)(p))
#define SET_PTR(p, ptr) (*(unsigned long *)(p) = (unsigned long)(ptr))

#define PRED_PTR(bp)    ((char *)(bp))
#define SUCC_PTR(bp)    ((char *)(bp) + WSIZE)
#define PRED(bp)        ((char *)GET_PTR(PRED_PTR(bp)))
#define SUCC(bp)        ((char *)GET_PTR(SUCC_PTR(bp)))

// 空闲链表头尾指针
static char *free_list_head = NULL;
static char *free_list_tail = NULL;

// 插入到空闲链表头部
static void insert_free_block(void *bp) {
    SET_PTR(PRED_PTR(bp), NULL);
    SET_PTR(SUCC_PTR(bp), free_list_head);
    if (free_list_head != NULL)
        SET_PTR(PRED_PTR(free_list_head), bp);
    free_list_head = bp;
}

// 从空闲链表删除
static void remove_free_block(void *bp) {
    if (PRED(bp) != NULL)
        SET_PTR(SUCC_PTR(PRED(bp)), SUCC(bp));
    else
        free_list_head = SUCC(bp);

    if (SUCC(bp) != NULL)
        SET_PTR(PRED_PTR(SUCC(bp)), PRED(bp));
}
```

### 4.3 修改后的合并函数

```c
static void *coalesce(void *bp) {
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {
        // 情况1: 直接插入
        insert_free_block(bp);
    }
    else if (prev_alloc && !next_alloc) {
        // 情况2: 与后一块合并
        remove_free_block(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        insert_free_block(bp);
    }
    else if (!prev_alloc && next_alloc) {
        // 情况3: 与前一块合并
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    else {
        // 情况4: 与前后都合并
        remove_free_block(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
                GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }

    return bp;
}
```

---

## 5. 分离适配实现（推荐）

### 5.1 设计思路

```c
// 按大小类分离的空闲链表
// 大小类: 16-31, 32-63, 64-127, ..., 4096+

#define SIZE_CLASS_COUNT 20

static char *size_class_heads[SIZE_CLASS_COUNT];

// 获取大小类索引
static int get_size_class(size_t size) {
    int index = 0;
    size = (size - 1) >> 4;  // 除以16
    while (size && index < SIZE_CLASS_COUNT - 1) {
        size >>= 1;
        index++;
    }
    return index;
}
```

### 5.2 完整分离适配实现

```c
#define ALIGNMENT 16
#define MIN_BLOCK_SIZE 32
#define SIZE_CLASS_COUNT 15

static char *heap_listp;
static char *size_class_heads[SIZE_CLASS_COUNT];

// 获取大小类
static int get_size_class(size_t size) {
    int i = 0;
    size_t sc = 32;
    while (size > sc && i < SIZE_CLASS_COUNT - 1) {
        sc <<= 1;
        i++;
    }
    return i;
}

// 插入到对应大小类
static void insert_block(void *bp) {
    size_t size = GET_SIZE(HDRP(bp));
    int class = get_size_class(size);

    void *head = size_class_heads[class];
    PRED(bp) = NULL;
    SUCC(bp) = head;
    if (head != NULL)
        PRED(head) = bp;
    size_class_heads[class] = bp;
}

// 删除块
static void remove_block(void *bp) {
    size_t size = GET_SIZE(HDRP(bp));
    int class = get_size_class(size);

    if (PRED(bp) != NULL)
        SUCC(PRED(bp)) = SUCC(bp);
    else
        size_class_heads[class] = SUCC(bp);

    if (SUCC(bp) != NULL)
        PRED(SUCC(bp)) = PRED(bp);
}

// 在大小类中搜索
static void *find_fit(size_t asize) {
    int class = get_size_class(asize);

    for (int i = class; i < SIZE_CLASS_COUNT; i++) {
        void *bp = size_class_heads[i];
        while (bp != NULL) {
            if (GET_SIZE(HDRP(bp)) >= asize)
                return bp;
            bp = SUCC(bp);
        }
    }
    return NULL;
}
```

---

## 6. 性能优化技巧

### 6.1 提高吞吐量

```c
// 1. 使用显式空闲链表 - O(1)查找vs O(n)
// 2. 使用分离适配 - 减少搜索范围
// 3. 延迟合并 - 只在需要时合并
// 4. 使用下次适配 - 减少搜索时间
```

### 6.2 提高利用率

```c
// 1. 分割策略 - 只在剩余足够大时分割
// 2. 边界标记合并 - 立即合并相邻空闲块
// 3. 考虑重新分配优化 - 原地扩展如果可能

// 优化的realloc
void *mm_realloc(void *ptr, size_t size) {
    if (ptr == NULL) return mm_malloc(size);
    if (size == 0) { mm_free(ptr); return NULL; }

    size_t old_size = GET_SIZE(HDRP(ptr));
    size_t asize = ALIGN(size + DSIZE);

    // 如果新大小更小或相同，可能不需要移动
    if (asize <= old_size) {
        // 如果可以分割
        if (old_size - asize >= MIN_BLOCK_SIZE) {
            PUT(HDRP(ptr), PACK(asize, 1));
            PUT(FTRP(ptr), PACK(asize, 1));
            void *next = NEXT_BLKP(ptr);
            PUT(HDRP(next), PACK(old_size - asize, 0));
            PUT(FTRP(next), PACK(old_size - asize, 0));
            coalesce(next);
        }
        return ptr;
    }

    // 检查下一块是否空闲且足够大
    void *next = NEXT_BLKP(ptr);
    if (!GET_ALLOC(HDRP(next)) &&
        old_size + GET_SIZE(HDRP(next)) >= asize) {
        remove_block(next);
        size_t total = old_size + GET_SIZE(HDRP(next));
        PUT(HDRP(ptr), PACK(total, 1));
        PUT(FTRP(ptr), PACK(total, 1));
        if (total - asize >= MIN_BLOCK_SIZE) {
            // 分割
            PUT(HDRP(ptr), PACK(asize, 1));
            PUT(FTRP(ptr), PACK(asize, 1));
            next = NEXT_BLKP(ptr);
            PUT(HDRP(next), PACK(total - asize, 0));
            PUT(FTRP(next), PACK(total - asize, 0));
            insert_block(next);
        }
        return ptr;
    }

    // 否则分配新块并复制
    void *new_ptr = mm_malloc(size);
    memcpy(new_ptr, ptr, old_size - DSIZE);
    mm_free(ptr);
    return new_ptr;
}
```

---

## 7. 测试方法

```bash
# 编译和测试
cd malloclab-handout
make

# 使用mdriver测试
./mdriver -V

# 特定测试
./mdriver -V -f short1-bal.rep
./mdriver -V -f coalescing-bal.rep

# 生成成绩
./mdriver

# 调试
make debug
./mdriver -D
```

---

**创建日期**: 2026-03-19


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
