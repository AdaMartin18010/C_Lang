/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\01_Malloc_Physics.md
 * Line: 618
 * Language: c
 * Block ID: 95775e56
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
