/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\01_Malloc_Physics.md
 * Line: 557
 * Language: c
 * Block ID: 46c6309a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
