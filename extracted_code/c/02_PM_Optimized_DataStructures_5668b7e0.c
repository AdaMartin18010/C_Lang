/*
 * Auto-generated from: 03_System_Technology_Domains\12_Persistent_Memory\02_PM_Optimized_DataStructures.md
 * Line: 317
 * Language: c
 * Block ID: 5668b7e0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 分裂节点 */
static TOID(struct btree_node) btree_split(PMEMobjpool *pop,
                                           TOID(struct btree_root) root,
                                           TOID(struct btree_node) parent,
                                           TOID(struct btree_node) node,
                                           int idx) {
    TX_BEGIN(pop) {
        /* 分配新节点 */
        TOID(struct btree_node) new_node = TX_NEW(struct btree_node);
        struct btree_node *n = D_RW(node);
        struct btree_node *new_n = D_RW(new_node);

        int mid = n->nkeys / 2;

        /* 复制一半数据到新节点 */
        if (n->flags & LEAF_NODE) {
            new_n->flags = LEAF_NODE;
            new_n->nkeys = n->nkeys - mid;
            memcpy(new_n->leaf.keys, n->leaf.keys + mid,
                   new_n->nkeys * BTREE_KEY_LEN);
            memcpy(new_n->leaf.values, n->leaf.values + mid,
                   new_n->nkeys * sizeof(PMEMoid));

            /* 更新链表 */
            TX_SET(new_node, next, n->next);
            TX_SET(node, next, new_node);
        } else {
            /* 内部节点分裂... */
        }

        TX_SET(node, nkeys, mid);

        /* 插入父节点 */
        if (OID_IS_NULL(parent.oid)) {
            /* 创建新根 */
            TOID(struct btree_node) new_root = TX_NEW(struct btree_node);
            /* ... */
            TX_SET(root, root, new_root);
            TX_SET(root, height, D_RO(root)->height + 1);
        } else {
            /* 插入到现有父节点... */
        }

        return new_node;
    } TX_END

    return OID_NULL;
}

/* 插入键值对 */
int btree_insert(PMEMobjpool *pop, TOID(struct btree_root) root,
                 const char *key, PMEMoid value) {
    TX_BEGIN(pop) {
        TOID(struct btree_node) node = D_RO(root)->root;
        TOID(struct btree_node) parent = OID_NULL;

        /* 找到叶子节点 */
        while (!(D_RO(node)->flags & LEAF_NODE)) {
            parent = node;
            /* 选择子节点... */
        }

        struct btree_node *leaf = D_RW(node);

        /* 检查是否需要分裂 */
        if (leaf->nkeys >= BTREE_ORDER) {
            TOID(struct btree_node) new_leaf = btree_split(pop, root, parent,
                                                           node, 0);
            /* 确定插入哪个节点... */
        }

        /* 插入到叶子 */
        TX_ADD(node);
        int pos = find_insert_pos(leaf, key);
        memmove(leaf->leaf.keys + pos + 1, leaf->leaf.keys + pos,
                (leaf->nkeys - pos) * BTREE_KEY_LEN);
        memmove(leaf->leaf.values + pos + 1, leaf->leaf.values + pos,
                (leaf->nkeys - pos) * sizeof(PMEMoid));

        memcpy(leaf->leaf.keys[pos], key, BTREE_KEY_LEN);
        leaf->leaf.values[pos] = value;
        leaf->nkeys++;

        TX_SET(root, size, D_RO(root)->size + 1);

    } TX_ONABORT {
        return -1;
    } TX_END

    return 0;
}
