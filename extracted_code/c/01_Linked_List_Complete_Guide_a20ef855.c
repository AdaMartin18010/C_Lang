/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 1487
 * Language: c
 * Block ID: a20ef855
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 内存池: 预先分配一批节点，减少malloc开销
 */
#define POOL_SIZE 1024

typedef struct NodePool {
    Node pool[POOL_SIZE];
    Node* free_list;
    int used;
} NodePool;

// 初始化内存池
void pool_init(NodePool* pool) {
    pool->free_list = nullptr;
    for (int i = POOL_SIZE - 1; i >= 0; i--) {
        pool->pool[i].next = pool->free_list;
        pool->free_list = &pool->pool[i];
    }
    pool->used = 0;
}

// 从内存池分配节点
[[nodiscard]]
Node* pool_alloc(NodePool* pool, int data) {
    if (pool->free_list == nullptr) {
        fprintf(stderr, "Error: Pool exhausted\n");
        return nullptr;
    }

    Node* node = pool->free_list;
    pool->free_list = node->next;

    node->data = data;
    node->next = nullptr;
    pool->used++;

    return node;
}

// 归还节点到内存池
void pool_free(NodePool* pool, Node* node) {
    node->next = pool->free_list;
    pool->free_list = node;
    pool->used--;
}
