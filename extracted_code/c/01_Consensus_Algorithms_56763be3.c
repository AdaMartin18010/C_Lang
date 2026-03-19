/*
 * Auto-generated from: 07_Modern_Toolchain\10_Distributed_Systems\01_Consensus_Algorithms.md
 * Line: 230
 * Language: c
 * Block ID: 56763be3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 一致性哈希环 */

typedef struct node {
    uint32_t hash;
    char id[64];
    struct node* next;
} ring_node_t;

typedef struct {
    ring_node_t* head;
    int virtual_nodes;  /* 每个物理节点的虚拟节点数 */
} hash_ring_t;

uint32_t hash_key(const char* key) {
    /* MurmurHash3 */
    return murmur3_32(key, strlen(key), 0);
}

void ring_add_node(hash_ring_t* ring, const char* node_id) {
    /* 添加虚拟节点 */
    for (int i = 0; i < ring->virtual_nodes; i++) {
        char vnode_key[128];
        snprintf(vnode_key, sizeof(vnode_key), "%s#%d", node_id, i);

        ring_node_t* node = malloc(sizeof(ring_node_t));
        node->hash = hash_key(vnode_key);
        strncpy(node->id, node_id, sizeof(node->id));

        /* 插入有序链表 */
        insert_sorted(ring, node);
    }
}

const char* ring_get_node(hash_ring_t* ring, const char* key) {
    uint32_t h = hash_key(key);

    /* 找到第一个hash >= h的节点 */
    ring_node_t* node = ring->head;
    while (node && node->hash < h) {
        node = node->next;
    }

    if (!node) {
        /* 回绕到第一个 */
        node = ring->head;
    }

    return node->id;
}

/* 获取key的N个副本位置 */
void ring_get_nodes(hash_ring_t* ring, const char* key,
                    const char** nodes, int n) {
    uint32_t h = hash_key(key);
    ring_node_t* node = find_first_ge(ring, h);

    for (int i = 0; i < n; i++) {
        if (!node) node = ring->head;
        nodes[i] = node->id;

        /* 跳过同一物理节点的其他虚拟节点 */
        const char* current = node->id;
        do {
            node = node->next;
        } while (node && strcmp(node->id, current) == 0);
    }
}
