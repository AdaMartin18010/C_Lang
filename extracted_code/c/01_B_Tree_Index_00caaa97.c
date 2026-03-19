/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\01_B_Tree_Index.md
 * Line: 755
 * Language: c
 * Block ID: 00caaa97
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ==================== 节点分裂 ==================== */

/*
 * 分裂叶子节点
 * 将满叶子节点分裂为两个节点
 */
static int btree_split_leaf(BTree* tree, BTreeNode* leaf,
                           SearchPath* path, int leaf_depth) {
    assert(leaf->header.num_keys >= tree->order);

    PageId new_page_id;
    BTreeNode* new_leaf = node_pool_alloc(&tree->node_pool, &new_page_id);
    if (!new_leaf) return BTREE_ERR_NOMEM;

    /* 计算分裂点 */
    int mid = leaf->header.num_keys / 2;
    int move_count = leaf->header.num_keys - mid;

    /* 初始化新叶子节点 */
    new_leaf->header.magic = BTREE_NODE_MAGIC;
    new_leaf->header.flags = 0;
    new_leaf->header.type = NODE_TYPE_LEAF;
    new_leaf->header.level = 0;
    new_leaf->header.num_keys = move_count;
    new_leaf->header.parent = leaf->header.parent;
    new_leaf->header.next = leaf->header.next;
    new_leaf->header.prev = tree->root_page_id;  /* 临时 */

    /* 复制后半部分到新节点 */
    memcpy(&new_leaf->entries.leaf[0], &leaf->entries.leaf[mid],
           move_count * sizeof(LeafEntry));

    /* 更新原节点 */
    leaf->header.num_keys = mid;
    leaf->header.next = new_page_id;

    /* 更新新节点的prev指针 */
    new_leaf->header.prev = tree->root_page_id;  /* 需要修正 */

    /* 获取分隔键 */
    BTreeKey* separator_key = &new_leaf->entries.leaf[0].key;

    /* 更新父节点 */
    if (leaf_depth == 0) {
        /* 根节点分裂，创建新根 */
        PageId new_root_id;
        BTreeNode* new_root = node_pool_alloc(&tree->node_pool, &new_root_id);
        if (!new_root) {
            node_pool_free(&tree->node_pool, new_page_id);
            return BTREE_ERR_NOMEM;
        }

        PageId old_root_id = tree->root_page_id;

        new_root->header.magic = BTREE_NODE_MAGIC;
        new_root->header.flags = 0;
        new_root->header.type = NODE_TYPE_ROOT;
        new_root->header.level = 1;
        new_root->header.num_keys = 1;
        new_root->header.parent = 0;
        new_root->header.next = 0;
        new_root->header.prev = 0;

        new_root->entries.internal[0].key = *separator_key;
        new_root->entries.internal[0].child = old_root_id;
        new_root->entries.internal[1].key = *separator_key;
        new_root->entries.internal[1].child = new_page_id;
        new_root->header.num_keys = 2;

        /* 修正 */
        new_root->header.num_keys = 1;

        leaf->header.parent = new_root_id;
        new_leaf->header.parent = new_root_id;
        new_leaf->header.type = NODE_TYPE_LEAF;

        tree->root = new_root;
        tree->root_page_id = new_root_id;
        tree->stats.height++;

        atomic_fetch_add(&tree->stats.splits, 1);
    } else {
        /* 非根节点，将分隔键插入父节点 */
        BTreeNode* parent = path->nodes[leaf_depth - 1];
        int insert_idx = path->indices[leaf_depth - 1];

        if (insert_idx < 0) insert_idx = -insert_idx - 1;

        /* 移动为新键腾出空间 */
        if (insert_idx < parent->header.num_keys) {
            memmove(&parent->entries.internal[insert_idx + 1],
                    &parent->entries.internal[insert_idx],
                    (parent->header.num_keys - insert_idx) * sizeof(InternalEntry));
        }

        parent->entries.internal[insert_idx].key = *separator_key;
        parent->entries.internal[insert_idx].child = new_page_id;
        parent->header.num_keys++;

        new_leaf->header.parent = tree->root_page_id;  /* 需要修正 */

        atomic_fetch_add(&tree->stats.splits, 1);

        /* 递归检查父节点是否需要分裂 */
        if (parent->header.num_keys >= tree->order) {
            /* 简化处理：暂不实现级联分裂 */
        }
    }

    return BTREE_OK;
}

/* ==================== 插入实现 ==================== */

int btree_insert(BTree* tree, const uint8_t* key, uint16_t key_len,
                 const uint8_t* value, uint16_t value_len) {
    if (!tree || !key || key_len == 0 || key_len > BTREE_KEY_SIZE ||
        !value || value_len > BTREE_VALUE_SIZE) {
        return BTREE_ERR_INVALID;
    }

    /* 获取写锁 */
    pthread_rwlock_wrlock(&tree->tree_lock.lock);

    /* 处理空树情况 */
    if (tree->root == NULL || tree->root->header.num_keys == 0) {
        PageId root_id;
        BTreeNode* root = node_pool_alloc(&tree->node_pool, &root_id);
        if (!root) {
            pthread_rwlock_unlock(&tree->tree_lock.lock);
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
    }

    /* 搜索插入位置 */
    SearchPath path;
    int result = btree_search_path(tree, key, key_len, &path);
    if (result != BTREE_OK) {
        pthread_rwlock_unlock(&tree->tree_lock.lock);
        return result;
    }

    BTreeNode* leaf = path.nodes[path.depth - 1];
    int idx = path.indices[path.depth - 1];

    /* 检查是否已存在 */
    if (idx >= 0) {
        pthread_rwlock_unlock(&tree->tree_lock.lock);
        return BTREE_ERR_DUPLICATE;
    }

    idx = -idx - 1;

    /* 为新条目腾出空间 */
    if (idx < leaf->header.num_keys) {
        memmove(&leaf->entries.leaf[idx + 1],
                &leaf->entries.leaf[idx],
                (leaf->header.num_keys - idx) * sizeof(LeafEntry));
    }

    /* 插入新条目 */
    LeafEntry* entry = &leaf->entries.leaf[idx];
    entry->key.length = key_len;
    entry->key.prefix_len = 0;
    memcpy(entry->key.data, key, key_len);
    entry->value_len = value_len;
    memcpy(entry->value, value, value_len);

    leaf->header.num_keys++;

    /* 检查是否需要分裂 */
    if (leaf->header.num_keys >= tree->order) {
        result = btree_split_leaf(tree, leaf, &path, path.depth - 1);
    }

    if (result == BTREE_OK) {
        atomic_fetch_add(&tree->stats.inserts, 1);
    }

    pthread_rwlock_unlock(&tree->tree_lock.lock);
    return result;
}

/* ==================== 更新实现 ==================== */

int btree_update(BTree* tree, const uint8_t* key, uint16_t key_len,
                 const uint8_t* value, uint16_t value_len) {
    if (!tree || !key || key_len == 0 || key_len > BTREE_KEY_SIZE ||
        !value || value_len > BTREE_VALUE_SIZE) {
        return BTREE_ERR_INVALID;
    }

    pthread_rwlock_wrlock(&tree->tree_lock.lock);

    SearchPath path;
    int result = btree_search_path(tree, key, key_len, &path);

    if (result == BTREE_OK) {
        BTreeNode* leaf = path.nodes[path.depth - 1];
        int idx = path.indices[path.depth - 1];

        if (idx >= 0) {
            /* 更新值 */
            LeafEntry* entry = &leaf->entries.leaf[idx];
            entry->value_len = value_len;
            memcpy(entry->value, value, value_len);
        } else {
            result = BTREE_ERR_NOTFOUND;
        }
    }

    pthread_rwlock_unlock(&tree->tree_lock.lock);
    return result;
}
