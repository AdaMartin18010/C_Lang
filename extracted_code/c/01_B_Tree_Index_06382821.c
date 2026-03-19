/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\01_B_Tree_Index.md
 * Line: 987
 * Language: c
 * Block ID: 06382821
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ==================== 借用与合并 ==================== */

/*
 * 从左兄弟借用键
 */
static int btree_borrow_from_left(BTreeNode* leaf, BTreeNode* left_sibling,
                                  BTreeNode* parent, int parent_idx) {
    if (left_sibling->header.num_keys <= BTREE_MIN_KEYS) {
        return BTREE_ERR_INVALID;
    }

    /* 移动父节点中的键到叶子节点开头 */
    memmove(&leaf->entries.leaf[1], &leaf->entries.leaf[0],
            leaf->header.num_keys * sizeof(LeafEntry));

    /* 从左兄弟借最后一个键 */
    int borrow_idx = left_sibling->header.num_keys - 1;
    leaf->entries.leaf[0] = left_sibling->entries.leaf[borrow_idx];
    leaf->header.num_keys++;
    left_sibling->header.num_keys--;

    /* 更新父节点的分隔键 */
    parent->entries.internal[parent_idx].key = leaf->entries.leaf[0].key;

    return BTREE_OK;
}

/*
 * 从右兄弟借用键
 */
static int btree_borrow_from_right(BTreeNode* leaf, BTreeNode* right_sibling,
                                   BTreeNode* parent, int parent_idx) {
    if (right_sibling->header.num_keys <= BTREE_MIN_KEYS) {
        return BTREE_ERR_INVALID;
    }

    /* 借右兄弟的第一个键 */
    leaf->entries.leaf[leaf->header.num_keys] = right_sibling->entries.leaf[0];
    leaf->header.num_keys++;

    /* 右兄弟前移 */
    memmove(&right_sibling->entries.leaf[0],
            &right_sibling->entries.leaf[1],
            (right_sibling->header.num_keys - 1) * sizeof(LeafEntry));
    right_sibling->header.num_keys--;

    /* 更新父节点的分隔键 */
    parent->entries.internal[parent_idx + 1].key = right_sibling->entries.leaf[0].key;

    return BTREE_OK;
}

/*
 * 与左兄弟合并
 */
static int btree_merge_with_left(BTreeNode* leaf, BTreeNode* left_sibling,
                                 BTreeNode* parent, int parent_idx) {
    /* 将所有键移动到左兄弟 */
    memcpy(&left_sibling->entries.leaf[left_sibling->header.num_keys],
           &leaf->entries.leaf[0],
           leaf->header.num_keys * sizeof(LeafEntry));
    left_sibling->header.num_keys += leaf->header.num_keys;
    left_sibling->header.next = leaf->header.next;

    /* 从父节点删除指向当前叶子的条目 */
    if (parent_idx < parent->header.num_keys - 1) {
        memmove(&parent->entries.internal[parent_idx],
                &parent->entries.internal[parent_idx + 1],
                (parent->header.num_keys - parent_idx - 1) * sizeof(InternalEntry));
    }
    parent->header.num_keys--;

    return BTREE_OK;
}

/*
 * 与右兄弟合并
 */
static int btree_merge_with_right(BTreeNode* leaf, BTreeNode* right_sibling,
                                  BTreeNode* parent, int parent_idx) {
    /* 将右兄弟的键移动到当前叶子 */
    memcpy(&leaf->entries.leaf[leaf->header.num_keys],
           &right_sibling->entries.leaf[0],
           right_sibling->header.num_keys * sizeof(LeafEntry));
    leaf->header.num_keys += right_sibling->header.num_keys;
    leaf->header.next = right_sibling->header.next;

    /* 从父节点删除指向右兄弟的条目 */
    if (parent_idx + 1 < parent->header.num_keys) {
        memmove(&parent->entries.internal[parent_idx + 1],
                &parent->entries.internal[parent_idx + 2],
                (parent->header.num_keys - parent_idx - 2) * sizeof(InternalEntry));
    }
    parent->header.num_keys--;

    return BTREE_OK;
}

/* ==================== 删除实现 ==================== */

int btree_delete(BTree* tree, const uint8_t* key, uint16_t key_len) {
    if (!tree || !key || key_len == 0 || key_len > BTREE_KEY_SIZE) {
        return BTREE_ERR_INVALID;
    }

    pthread_rwlock_wrlock(&tree->tree_lock.lock);

    SearchPath path;
    int result = btree_search_path(tree, key, key_len, &path);

    if (result != BTREE_OK) {
        pthread_rwlock_unlock(&tree->tree_lock.lock);
        return result;
    }

    BTreeNode* leaf = path.nodes[path.depth - 1];
    int idx = path.indices[path.depth - 1];

    /* 键不存在 */
    if (idx < 0) {
        pthread_rwlock_unlock(&tree->tree_lock.lock);
        return BTREE_ERR_NOTFOUND;
    }

    /* 从叶子节点删除 */
    if (idx < leaf->header.num_keys - 1) {
        memmove(&leaf->entries.leaf[idx],
                &leaf->entries.leaf[idx + 1],
                (leaf->header.num_keys - idx - 1) * sizeof(LeafEntry));
    }
    leaf->header.num_keys--;

    atomic_fetch_add(&tree->stats.deletes, 1);

    /* 检查是否需要重新平衡 */
    if (leaf->header.num_keys < tree->min_keys && path.depth > 1) {
        /* 简化实现：仅标记，实际重新平衡较复杂 */
        atomic_fetch_add(&tree->stats.merges, 1);
    }

    pthread_rwlock_unlock(&tree->tree_lock.lock);
    return BTREE_OK;
}
