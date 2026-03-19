/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\01_B_Tree_Index.md
 * Line: 1460
 * Language: c
 * Block ID: 2117e5d1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ==================== B+树生命周期 ==================== */

int btree_init(BTree* tree, uint16_t order) {
    if (!tree || order < 4) return BTREE_ERR_INVALID;

    memset(tree, 0, sizeof(BTree));

    tree->order = order;
    tree->min_keys = order / 2;
    tree->use_optimistic = false;

    /* 初始化节点池 */
    int result = node_pool_init(&tree->node_pool, BTREE_NODE_POOL_SIZE);
    if (result != BTREE_OK) return result;

    /* 初始化页分配器 */
    result = page_allocator_init(&tree->page_allocator);
    if (result != BTREE_OK) {
        node_pool_cleanup(&tree->node_pool);
        return result;
    }

    /* 初始化锁 */
    result = btree_rwlock_init(&tree->tree_lock);
    if (result != BTREE_OK) {
        node_pool_cleanup(&tree->node_pool);
        page_allocator_cleanup(&tree->page_allocator);
        return result;
    }

    /* 创建根节点 */
    PageId root_id;
    BTreeNode* root = node_pool_alloc(&tree->node_pool, &root_id);
    if (!root) {
        btree_rwlock_destroy(&tree->tree_lock);
        node_pool_cleanup(&tree->node_pool);
        page_allocator_cleanup(&tree->page_allocator);
        return BTREE_ERR_NOMEM;
    }

    root->header.magic = BTREE_NODE_MAGIC;
    root->header.flags = 0;
    root->header.type = NODE_TYPE_LEAF;
    root->header.level = 0;
    root->header.num_keys = 0;
    root->header.parent = 0;
    root->header.next = 0;
    root->header.prev = 0;

    tree->root = root;
    tree->root_page_id = root_id;
    tree->stats.height = 1;

    return BTREE_OK;
}

BTree* btree_create(uint16_t order) {
    BTree* tree = (BTree*)malloc(sizeof(BTree));
    if (!tree) return NULL;

    if (btree_init(tree, order) != BTREE_OK) {
        free(tree);
        return NULL;
    }

    return tree;
}

void btree_cleanup(BTree* tree) {
    if (!tree) return;

    btree_rwlock_destroy(&tree->tree_lock);
    node_pool_cleanup(&tree->node_pool);
    page_allocator_cleanup(&tree->page_allocator);
}

void btree_destroy(BTree* tree) {
    if (tree) {
        btree_cleanup(tree);
        free(tree);
    }
}
