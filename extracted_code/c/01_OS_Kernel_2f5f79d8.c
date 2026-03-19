/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\01_OS_Kernel.md
 * Line: 196
 * Language: c
 * Block ID: 2f5f79d8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Slab分配器用于分配固定大小的对象

struct kmem_cache {
    char *name;              // 缓存名称
    size_t object_size;      // 对象大小
    size_t align;            // 对齐要求
    struct slab *partial;    // 部分使用的slab
    struct slab *full;       // 满的slab
    struct slab *free;       // 空闲slab
    void (*ctor)(void *);    // 构造函数
};

struct slab {
    struct list_head list;
    void *s_mem;             // 第一个对象
    unsigned int inuse;      // 已分配对象数
    void **freelist;         // 空闲对象链表
};

// 创建缓存
struct kmem_cache *kmem_cache_create(
    const char *name,
    size_t size,
    size_t align,
    void (*ctor)(void *)
) {
    struct kmem_cache *cache = alloc_bootmem(sizeof(*cache));
    cache->name = name;
    cache->object_size = size;
    cache->align = align;
    cache->ctor = ctor;
    INIT_LIST_HEAD(&cache->partial);
    INIT_LIST_HEAD(&cache->full);
    INIT_LIST_HEAD(&cache->free);
    return cache;
}

// 分配对象
void *kmem_cache_alloc(struct kmem_cache *cache) {
    struct slab *slab;
    void *obj = NULL;

    // 优先从partial slab分配
    if (!list_empty(&cache->partial)) {
        slab = list_entry(cache->partial.next, struct slab, list);
    } else if (!list_empty(&cache->free)) {
        // 使用空闲slab
        slab = list_entry(cache->free.next, struct slab, list);
        list_move(&slab->list, &cache->partial);
    } else {
        // 创建新slab
        slab = alloc_new_slab(cache);
        list_add(&slab->list, &cache->partial);
    }

    // 从slab分配对象
    obj = slab->freelist;
    slab->freelist = *(void **)obj;
    slab->inuse++;

    if (slab->inuse == objects_per_slab(cache)) {
        list_move(&slab->list, &cache->full);
    }

    return obj;
}
