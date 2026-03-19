/*
 * Auto-generated from: 03_System_Technology_Domains\12_Persistent_Memory\02_PM_Optimized_DataStructures.md
 * Line: 250
 * Language: c
 * Block ID: 93545b87
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* B+树节点 - 针对PM优化 */
#define BTREE_ORDER 32        /* 减少分裂频率 */
#define BTREE_KEY_LEN 32      /* 固定长度键 */

struct btree_node {
    TOID(struct btree_node) parent;    /* 父节点 */
    TOID(struct btree_node) next;      /* 叶子链表 */

    uint16_t nkeys;           /* 键数量 */
    uint16_t flags;           /* 节点类型标志 */

    union {
        /* 内部节点 */
        struct {
            char keys[BTREE_ORDER][BTREE_KEY_LEN];
            TOID(struct btree_node) children[BTREE_ORDER + 1];
        } internal;

        /* 叶子节点 */
        struct {
            char keys[BTREE_ORDER][BTREE_KEY_LEN];
            PMEMoid values[BTREE_ORDER];   /* 值OID */
        } leaf;
    };
};

/* 树根 */
struct btree_root {
    TOID(struct btree_node) root;
    uint64_t size;
    uint32_t height;
};

/* 无锁读取 - 不需要事务 */
PMEMoid btree_find(PMEMobjpool *pop, TOID(struct btree_root) root,
                   const char *key) {
    TOID(struct btree_node) node = D_RO(root)->root;

    while (!OID_IS_NULL(node.oid)) {
        const struct btree_node *n = D_RO(node);

        if (n->flags & LEAF_NODE) {
            /* 叶子节点 - 二分查找 */
            for (int i = 0; i < n->nkeys; i++) {
                if (memcmp(n->leaf.keys[i], key, BTREE_KEY_LEN) == 0) {
                    return n->leaf.values[i];
                }
            }
            return OID_NULL;
        } else {
            /* 内部节点 - 找到合适的子节点 */
            int i = 0;
            while (i < n->nkeys &&
                   memcmp(key, n->internal.keys[i], BTREE_KEY_LEN) > 0) {
                i++;
            }
            node = n->internal.children[i];
        }
    }

    return OID_NULL;
}
