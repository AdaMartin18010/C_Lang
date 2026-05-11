# Arena/Region分配器与Scoped Allocation

> **标准**: ISO/IEC 9899:2024 (C23) Section 7.22.3
> **前置知识**: [内存管理](../02_Core_Layer/02_Memory_Management.md), [抽象状态机](../01_Basic_Layer/00_Abstract_State_Machine.md)
> **后续延伸**: 自定义内存分配器, [所有权语义](00_Ownership_Semantics.md)
> **对标资源**: Modern C实践, Handmade Hero内存管理, Ginger Bill的Arena文章
> **难度**: 3/5 | **预计学习时间**: 2-3 小时

---

## 目录

1. [为什么需要Arena分配器](#一为什么需要arena)
2. [Arena基本原理](#二arena原理)
3. [基础Arena实现](#三基础实现)
4. [高级Arena模式](#四高级模式)
5. [Region-Based内存管理](#五region-based)
6. [Scratchpad/Arena池](#六scratchpad)
7. [与malloc的性能对比](#七性能对比)
8. [实际应用案例](#八应用案例)

---

## 一、为什么需要Arena

### 1.1 malloc/free的问题

| 问题 | 说明 |
|:-----|:-----|
| **碎片化** | 频繁分配/释放不同大小的对象导致内存碎片 |
| **开销** | 每次malloc/free需要元数据管理和锁竞争（多线程）|
| **泄漏风险** | 忘记free导致内存泄漏；提前free导致use-after-free |
| **缓存不友好** | 分配的内存可能分散在物理内存各处 |
| **无法批量释放** | 释放N个对象需要N次free调用 |

### 1.2 Arena的核心思想

> **一次性分配一大块内存，从中快速分配对象，最后一次性释放整个Arena。**

```
传统malloc/free:
  malloc(A) -> malloc(B) -> malloc(C) -> free(B) -> free(A) -> free(C)
  （6次操作，碎片化）

Arena模式:
  arena_create(1MB) -> alloc(A) -> alloc(B) -> alloc(C) -> arena_destroy()
  （2次系统调用，无碎片，批量释放）
```

### 1.3 Arena适用场景

| 场景 | 说明 |
|:-----|:-----|
| **编译器/解析器** | AST节点批量分配，编译完成后全部释放 |
| **游戏帧循环** | 每帧分配临时对象，帧末重置Arena |
| **HTTP请求处理** | 请求生命周期内分配，响应后释放整个Arena |
| **配置文件解析** | 解析过程中分配字符串和结构体，解析完成释放 |
| **临时计算** | 数学计算中的临时矩阵/向量 |

---

## 二、Arena原理

### 2.1 基本结构

```
Arena（内存池）
+------------------+------------------+------------------+
| 已分配对象A      | 已分配对象B      | 空闲空间         |
| (size=16)        | (size=24)        |                  |
+------------------+------------------+------------------+
^                  ^                  ^
|                  |                  |
base               ptr                end

分配新对象：ptr向前移动
重置Arena：ptr = base（批量释放）
销毁Arena：munmap/free整个内存块
```

### 2.2 内存对齐

所有分配必须满足对象的对齐要求（通常是8或16字节）：

```c
// 对齐宏
#define ALIGN_UP(ptr, align) \
    (((uintptr_t)(ptr) + (align) - 1) & ~((align) - 1))

// 示例
void* p = arena->ptr;
p = (void*)ALIGN_UP(p, alignof(int));  // 4字节对齐
p = (void*)ALIGN_UP(p, alignof(max_align_t));  // 最大对齐（通常16字节）
```

---

## 三、基础Arena实现

### 3.1 单块Arena

```c
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

typedef struct {
    char* base;      // 内存块起始
    char* ptr;       // 当前分配位置
    char* end;       // 内存块结束
} Arena;

// 创建Arena
Arena arena_create(size_t capacity) {
    Arena a;
    a.base = (char*)malloc(capacity);
    assert(a.base != NULL);
    a.ptr = a.base;
    a.end = a.base + capacity;
    return a;
}

// 从Arena分配内存
void* arena_alloc(Arena* a, size_t size, size_t align) {
    // 对齐当前指针
    char* p = (char*)(((uintptr_t)a->ptr + align - 1) & ~(align - 1));

    // 检查容量
    if (p + size > a->end) {
        return NULL;  // Arena溢出
    }

    a->ptr = p + size;
    return p;
}

// 重置Arena（批量释放所有对象）
void arena_reset(Arena* a) {
    a->ptr = a->base;
}

// 销毁Arena
void arena_destroy(Arena* a) {
    free(a->base);
    a->base = a->ptr = a->end = NULL;
}

// 获取已使用字节数
size_t arena_used(const Arena* a) {
    return (size_t)(a->ptr - a->base);
}

// 获取剩余字节数
size_t arena_available(const Arena* a) {
    return (size_t)(a->end - a->ptr);
}
```

### 3.2 使用示例

```c
#include <stdio.h>
#include <string.h>

typedef struct {
    int x, y;
} Point;

int main(void) {
    Arena arena = arena_create(1024);  // 1KB Arena

    // 分配对象
    Point* p1 = arena_alloc(&arena, sizeof(Point), alignof(Point));
    p1->x = 10; p1->y = 20;

    Point* p2 = arena_alloc(&arena, sizeof(Point), alignof(Point));
    p2->x = 30; p2->y = 40;

    // 分配字符串
    const char* msg = "Hello, Arena!";
    size_t len = strlen(msg) + 1;
    char* copy = arena_alloc(&arena, len, alignof(char));
    memcpy(copy, msg, len);

    printf("p1: (%d, %d)\n", p1->x, p1->y);
    printf("p2: (%d, %d)\n", p2->x, p2->y);
    printf("msg: %s\n", copy);
    printf("used: %zu bytes\n", arena_used(&arena));

    // 重置Arena（释放所有对象）
    arena_reset(&arena);
    // p1, p2, copy 现在不可访问！

    // 可以重新分配
    Point* p3 = arena_alloc(&arena, sizeof(Point), alignof(Point));
    p3->x = 100;

    arena_destroy(&arena);
    return 0;
}
```

### 3.3 多块Arena（支持动态增长）

```c
typedef struct ArenaBlock {
    struct ArenaBlock* next;
    char* base;
    char* ptr;
    char* end;
} ArenaBlock;

typedef struct {
    ArenaBlock* first;
    ArenaBlock* current;
    size_t block_size;
} MultiArena;

MultiArena multi_arena_create(size_t block_size) {
    MultiArena ma = {NULL, NULL, block_size};
    return ma;
}

void* multi_arena_alloc(MultiArena* ma, size_t size, size_t align) {
    // 尝试从当前块分配
    if (ma->current) {
        char* p = (char*)(((uintptr_t)ma->current->ptr + align - 1) & ~(align - 1));
        if (p + size <= ma->current->end) {
            ma->current->ptr = p + size;
            return p;
        }
    }

    // 分配新块
    size_t alloc_size = ma->block_size;
    if (size > alloc_size) alloc_size = size;

    ArenaBlock* block = malloc(sizeof(ArenaBlock) + alloc_size);
    block->base = (char*)(block + 1);
    block->ptr = block->base + size;
    block->end = block->base + alloc_size;
    block->next = NULL;

    if (ma->current) {
        ma->current->next = block;
    } else {
        ma->first = block;
    }
    ma->current = block;

    return block->base;
}

void multi_arena_reset(MultiArena* ma) {
    for (ArenaBlock* b = ma->first; b; b = b->next) {
        b->ptr = b->base;
    }
    ma->current = ma->first;
}

void multi_arena_destroy(MultiArena* ma) {
    ArenaBlock* b = ma->first;
    while (b) {
        ArenaBlock* next = b->next;
        free(b);
        b = next;
    }
    ma->first = ma->current = NULL;
}
```

---

## 四、高级Arena模式

### 4.1 临时Arena（Scratchpad）

为短生命周期计算提供临时存储：

```c
typedef struct {
    Arena* parent;
    char* save_ptr;
} TempArena;

TempArena temp_arena_begin(Arena* parent) {
    TempArena t = {parent, parent->ptr};
    return t;
}

void temp_arena_end(TempArena* t) {
    t->parent->ptr = t->save_ptr;  // 恢复到之前的位置
}

// 使用示例
void process_data(Arena* arena, const Data* input) {
    TempArena temp = temp_arena_begin(arena);

    // 分配临时缓冲区
    char* buffer = arena_alloc(temp.parent, input->size, 1);
    // ... 使用buffer处理数据 ...

    temp_arena_end(&temp);  // 自动释放所有临时分配
}
```

### 4.2 Arena标记（Checkpoint）

```c
typedef char* ArenaMarker;

ArenaMarker arena_mark(Arena* a) {
    return a->ptr;
}

void arena_rollback(Arena* a, ArenaMarker mark) {
    assert(mark >= a->base && mark <= a->ptr);
    a->ptr = mark;
}

// 使用示例
void parser(Arena* arena) {
    ArenaMarker checkpoint = arena_mark(arena);

    ASTNode* node = parse_expression(arena);
    if (node == NULL) {
        arena_rollback(arena, checkpoint);  // 回滚到解析前状态
        return;
    }

    // ... 继续处理 ...
}
```

### 4.3 子Arena

```c
typedef struct {
    Arena* parent;
    char* start;
    char* end;
} SubArena;

SubArena sub_arena_create(Arena* parent, size_t size) {
    void* mem = arena_alloc(parent, size, alignof(max_align_t));
    SubArena sa = {parent, mem, (char*)mem + size};
    return sa;
}
```

---

## 五、Region-Based内存管理

Region-Based是Arena的泛化概念，强调**对象生命周期与作用域绑定**。

### 5.1 Region概念

```
全局Region          长期存活对象（程序生命周期）
    |
    +-- 模块Region    模块级对象（模块加载期间）
    |       |
    |       +-- 函数Region  函数级对象（函数执行期间）
    |               |
    |               +-- 块Region  块级对象（块执行期间）
    |
    +-- 请求Region    HTTP请求/事务对象（请求处理期间）
```

### 5.2 C语言中的Region模拟

```c
// 使用GNU扩展的cleanup属性模拟Scoped Region
#ifdef __GNUC__
#define REGION(name, arena) \
    Arena* name __attribute__((cleanup(region_cleanup))) = arena

static inline void region_cleanup(Arena** a) {
    arena_reset(*a);
}
#else
#define REGION(name, arena) Arena* name = arena
#endif

// 使用示例（GNU C）
void process_request(Request* req) {
    Arena region_arena = arena_create(64 * 1024);  // 64KB
    REGION(ra, &region_arena);

    // 所有在此函数中分配的对象都会自动释放
    Response* resp = arena_alloc(ra, sizeof(Response), alignof(Response));
    char* body = arena_alloc(ra, req->body_size, 1);

    // ... 处理请求 ...

    // 函数返回时，region_arena自动重置（cleanup属性）
}
```

---

## 六、Scratchpad/Arena池

### 6.1 双缓冲Scratchpad

```c
#define SCRATCH_SIZE (64 * 1024)

__thread Arena scratch_arenas[2];
__thread int scratch_index = 0;

void scratch_init(void) {
    scratch_arenas[0] = arena_create(SCRATCH_SIZE);
    scratch_arenas[1] = arena_create(SCRATCH_SIZE);
}

Arena* scratch_begin(void) {
    return &scratch_arenas[scratch_index];
}

void scratch_end(void) {
    arena_reset(&scratch_arenas[scratch_index]);
    scratch_index = 1 - scratch_index;  // 切换缓冲区
}

// 使用：避免在临时分配中意外覆盖仍在使用的数据
void example(void) {
    Arena* s = scratch_begin();
    char* buf1 = arena_alloc(s, 1000, 1);

    // ... 使用buf1 ...

    scratch_end();  // 释放buf1
}
```

---

## 七、性能对比

### 7.1 基准测试

```c
#include <time.h>

#define N 100000
#define OBJ_SIZE 64

double benchmark_malloc_free() {
    void* ptrs[N];
    clock_t start = clock();
    for (int i = 0; i < N; i++) ptrs[i] = malloc(OBJ_SIZE);
    for (int i = 0; i < N; i++) free(ptrs[i]);
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

double benchmark_arena() {
    Arena a = arena_create(N * OBJ_SIZE);
    clock_t start = clock();
    for (int i = 0; i < N; i++) arena_alloc(&a, OBJ_SIZE, 8);
    arena_reset(&a);  // 一次性释放
    clock_t end = clock();
    arena_destroy(&a);
    return (double)(end - start) / CLOCKS_PER_SEC;
}
```

### 7.2 典型性能结果

| 操作 | malloc/free | Arena | 加速比 |
|:-----|:------------|:------|:-------|
| 分配10万个64字节对象 | ~15ms | ~1ms | **15x** |
| 释放10万个对象 | ~10ms | ~0ns（reset）| **无穷大** |
| 碎片化 | 严重 | 无 | - |
| 缓存局部性 | 差 | 极好 | - |

---

## 八、实际应用案例

### 8.1 编译器AST分配

```c
typedef struct ASTNode {
    enum { AST_INT, AST_ADD, AST_MUL } kind;
    struct ASTNode* left;
    struct ASTNode* right;
    int value;
} ASTNode;

typedef struct {
    Arena arena;
    ASTNode* root;
} Parser;

ASTNode* ast_new_int(Parser* p, int value) {
    ASTNode* n = arena_alloc(&p->arena, sizeof(ASTNode), alignof(ASTNode));
    n->kind = AST_INT;
    n->value = value;
    n->left = n->right = NULL;
    return n;
}

ASTNode* ast_new_binop(Parser* p, int kind, ASTNode* left, ASTNode* right) {
    ASTNode* n = arena_alloc(&p->arena, sizeof(ASTNode), alignof(ASTNode));
    n->kind = kind;
    n->left = left;
    n->right = right;
    return n;
}

// 解析完成后，一次性释放所有AST节点
void parser_destroy(Parser* p) {
    arena_destroy(&p->arena);  // 一个操作释放数千个节点
}
```

### 8.2 游戏帧分配器

```c
typedef struct {
    Arena frame_arena;
    uint64_t frame_count;
} GameState;

void game_update(GameState* gs) {
    arena_reset(&gs->frame_arena);  // 每帧开始时重置

    // 分配帧临时数据
    Particle* particles = arena_alloc(&gs->frame_arena,
                                      MAX_PARTICLES * sizeof(Particle),
                                      alignof(Particle));

    // 更新粒子、渲染等

    // 帧结束：所有临时数据自动释放
    gs->frame_count++;
}
```

---

## 参考资源

1. **Modern C** (Jens Gustedt) - 内存管理章节
2. **Handmade Hero** (Casey Muratori) - 每日内存分配模式
3. **Ginger Bill's Arena Allocator** - gingerbill.org/article/2019/02/01/memory-allocation-strategies-002/
4. **Untangling Lifetimes: The Arena Allocator** - rkaehn.github.io/2022/11/27/arena-allocator.html
5. **Region-Based Memory Management in C** - 学术研究论文

---

> 核心总结：Arena分配器通过"预分配大块内存 + 批量释放"的模式，解决了传统malloc/free的碎片化、开销和泄漏问题。它是编译器、游戏引擎、网络服务器等高性能C程序的核心内存管理技术。在现代C编程中，Arena不是malloc的替代，而是互补工具——两者结合使用可以兼顾灵活性和性能。

---

*文档状态: 初版完成 | 对齐: Modern C实践, Handmade Hero, Ginger Bill | 最后更新: 2026-05-11*
