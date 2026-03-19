/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\01_B_Tree_Index.md
 * Line: 424
 * Language: c
 * Block ID: a5b9a856
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * B+树实现 - 内存管理与工具函数
 */

#include "btree.h"
#include <stdio.h>
#include <time.h>

/* ==================== 错误码转字符串 ==================== */

const char* btree_error_string(int error) {
    switch (error) {
        case BTREE_OK:         return "Success";
        case BTREE_ERR_NOMEM:  return "Out of memory";
        case BTREE_ERR_NOTFOUND: return "Key not found";
        case BTREE_ERR_DUPLICATE: return "Duplicate key";
        case BTREE_ERR_IO:     return "I/O error";
        case BTREE_ERR_CORRUPT: return "Data corruption";
        case BTREE_ERR_LOCK:   return "Lock error";
        case BTREE_ERR_INVALID: return "Invalid parameter";
        default:               return "Unknown error";
    }
}

/* ==================== 节点池管理 ==================== */

static int node_pool_init(NodePool* pool, size_t initial_capacity) {
    pool->nodes = (BTreeNode*)calloc(initial_capacity, sizeof(BTreeNode));
    if (!pool->nodes) return BTREE_ERR_NOMEM;

    pool->free_list = (uint32_t*)malloc(initial_capacity * sizeof(uint32_t));
    if (!pool->free_list) {
        free(pool->nodes);
        return BTREE_ERR_NOMEM;
    }

    /* 初始化空闲列表 */
    for (size_t i = 0; i < initial_capacity; i++) {
        pool->free_list[i] = (uint32_t)i;
    }

    pool->capacity = initial_capacity;
    pool->used = 0;
    pthread_mutex_init(&pool->lock, NULL);

    return BTREE_OK;
}

static void node_pool_cleanup(NodePool* pool) {
    if (pool->nodes) {
        free(pool->nodes);
        pool->nodes = NULL;
    }
    if (pool->free_list) {
        free(pool->free_list);
        pool->free_list = NULL;
    }
    pthread_mutex_destroy(&pool->lock);
}

static BTreeNode* node_pool_alloc(NodePool* pool, PageId* page_id) {
    pthread_mutex_lock(&pool->lock);

    if (pool->used >= pool->capacity) {
        /* 扩展池 */
        size_t new_capacity = pool->capacity * 2;
        BTreeNode* new_nodes = (BTreeNode*)realloc(pool->nodes,
                                                    new_capacity * sizeof(BTreeNode));
        if (!new_nodes) {
            pthread_mutex_unlock(&pool->lock);
            return NULL;
        }

        uint32_t* new_free_list = (uint32_t*)realloc(pool->free_list,
                                                      new_capacity * sizeof(uint32_t));
        if (!new_free_list) {
            pthread_mutex_unlock(&pool->lock);
            return NULL;
        }

        /* 初始化新节点的空闲列表 */
        for (size_t i = pool->capacity; i < new_capacity; i++) {
            new_free_list[i] = (uint32_t)i;
        }

        pool->nodes = new_nodes;
        pool->free_list = new_free_list;
        pool->capacity = new_capacity;
    }

    uint32_t idx = pool->free_list[pool->used++];
    BTreeNode* node = &pool->nodes[idx];
    memset(node, 0, sizeof(BTreeNode));

    pthread_mutex_unlock(&pool->lock);

    *page_id = (PageId)idx + 1;  /* 页ID从1开始 */
    return node;
}

static void node_pool_free(NodePool* pool, PageId page_id) {
    if (page_id == 0) return;

    pthread_mutex_lock(&pool->lock);
    uint32_t idx = (uint32_t)(page_id - 1);
    if (pool->used > 0) {
        pool->free_list[--pool->used] = idx;
    }
    pthread_mutex_unlock(&pool->lock);
}

static BTreeNode* node_pool_get(NodePool* pool, PageId page_id) {
    if (page_id == 0 || page_id > (PageId)pool->capacity) return NULL;
    return &pool->nodes[page_id - 1];
}

/* ==================== 页分配器 ==================== */

static int page_allocator_init(PageAllocator* alloc) {
    alloc->next_page_id = 1;
    alloc->freed_capacity = 256;
    alloc->freed_count = 0;
    alloc->freed_pages = (PageId*)malloc(alloc->freed_capacity * sizeof(PageId));
    if (!alloc->freed_pages) return BTREE_ERR_NOMEM;

    pthread_mutex_init(&alloc->lock, NULL);
    return BTREE_OK;
}

static void page_allocator_cleanup(PageAllocator* alloc) {
    if (alloc->freed_pages) {
        free(alloc->freed_pages);
        alloc->freed_pages = NULL;
    }
    pthread_mutex_destroy(&alloc->lock);
}

static PageId page_allocator_alloc(PageAllocator* alloc) {
    pthread_mutex_lock(&alloc->lock);

    PageId page_id;
    if (alloc->freed_count > 0) {
        /* 优先使用回收的页 */
        page_id = alloc->freed_pages[--alloc->freed_count];
    } else {
        page_id = alloc->next_page_id++;
    }

    pthread_mutex_unlock(&alloc->lock);
    return page_id;
}

static void page_allocator_free(PageAllocator* alloc, PageId page_id) {
    if (page_id == 0) return;

    pthread_mutex_lock(&alloc->lock);

    if (alloc->freed_count >= alloc->freed_capacity) {
        /* 扩展回收数组 */
        size_t new_cap = alloc->freed_capacity * 2;
        PageId* new_pages = (PageId*)realloc(alloc->freed_pages,
                                              new_cap * sizeof(PageId));
        if (new_pages) {
            alloc->freed_pages = new_pages;
            alloc->freed_capacity = new_cap;
        }
    }

    if (alloc->freed_count < alloc->freed_capacity) {
        alloc->freed_pages[alloc->freed_count++] = page_id;
    }

    pthread_mutex_unlock(&alloc->lock);
}
