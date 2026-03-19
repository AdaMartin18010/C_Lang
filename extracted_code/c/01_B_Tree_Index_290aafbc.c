/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\01_B_Tree_Index.md
 * Line: 1137
 * Language: c
 * Block ID: 290aafbc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ==================== 迭代器实现 ==================== */

BTreeIterator* btree_iterator_create(BTree* tree,
                                     const uint8_t* start_key, uint16_t start_len,
                                     const uint8_t* end_key, uint16_t end_len,
                                     bool forward) {
    if (!tree) return NULL;

    BTreeIterator* iter = (BTreeIterator*)malloc(sizeof(BTreeIterator));
    if (!iter) return NULL;

    iter->tree = tree;
    iter->forward = forward;
    iter->valid = false;
    iter->current_index = 0;
    iter->current_node = NULL;

    /* 保存范围键 */
    if (start_key && start_len > 0 && start_len <= BTREE_KEY_SIZE) {
        iter->start_key.length = start_len;
        iter->start_key.prefix_len = 0;
        memcpy(iter->start_key.data, start_key, start_len);
    } else {
        iter->start_key.length = 0;
    }

    if (end_key && end_len > 0 && end_len <= BTREE_KEY_SIZE) {
        iter->end_key.length = end_len;
        iter->end_key.prefix_len = 0;
        memcpy(iter->end_key.data, end_key, end_len);
    } else {
        iter->end_key.length = 0;
    }

    /* 获取读锁并定位起始位置 */
    pthread_rwlock_rdlock(&tree->tree_lock.lock);

    if (tree->root && tree->root->header.num_keys > 0) {
        SearchPath path;

        if (iter->start_key.length > 0) {
            btree_search_path(tree, iter->start_key.data,
                            iter->start_key.length, &path);
        } else {
            /* 从头开始 */
            path.depth = 0;
            BTreeNode* node = tree->root;
            while (node && node->header.type != NODE_TYPE_LEAF &&
                   node->header.level > 0) {
                path.nodes[path.depth++] = node;
                PageId child_id = node->entries.internal[0].child;
                node = node_pool_get(&tree->node_pool, child_id);
            }
            path.nodes[path.depth++] = node;
            path.indices[path.depth - 1] = 0;
        }

        iter->current_node = path.nodes[path.depth - 1];
        int idx = path.indices[path.depth - 1];
        iter->current_index = (idx < 0) ? -idx - 1 : idx;
        iter->valid = (iter->current_node != NULL);
        iter->version = tree->stats.inserts;  /* 简化版本 */
    }

    pthread_rwlock_unlock(&tree->tree_lock.lock);

    return iter;
}

void btree_iterator_destroy(BTreeIterator* iter) {
    if (iter) {
        free(iter);
    }
}

bool btree_iterator_next(BTreeIterator* iter) {
    if (!iter || !iter->valid) return false;

    pthread_rwlock_rdlock(&iter->tree->tree_lock.lock);

    if (!iter->forward) {
        /* 切换为正向遍历 */
        iter->forward = true;
    }

    iter->current_index++;

    /* 检查是否需要移动到下一个节点 */
    while (iter->current_node &&
           iter->current_index >= iter->current_node->header.num_keys) {
        PageId next_id = iter->current_node->header.next;
        if (next_id == 0) {
            iter->valid = false;
            pthread_rwlock_unlock(&iter->tree->tree_lock.lock);
            return false;
        }
        iter->current_node = node_pool_get(&iter->tree->node_pool, next_id);
        iter->current_index = 0;
    }

    /* 检查是否超出结束键 */
    if (iter->end_key.length > 0 && iter->current_node) {
        LeafEntry* entry = &iter->current_node->entries.leaf[iter->current_index];
        if (key_compare(&entry->key, &iter->end_key) > 0) {
            iter->valid = false;
            pthread_rwlock_unlock(&iter->tree->tree_lock.lock);
            return false;
        }
    }

    pthread_rwlock_unlock(&iter->tree->tree_lock.lock);
    return iter->valid && iter->current_node != NULL;
}

bool btree_iterator_prev(BTreeIterator* iter) {
    if (!iter || !iter->valid) return false;

    pthread_rwlock_rdlock(&iter->tree->tree_lock.lock);

    if (iter->forward) {
        iter->forward = false;
    }

    iter->current_index--;

    /* 检查是否需要移动到上一个节点 */
    while (iter->current_node && iter->current_index < 0) {
        PageId prev_id = iter->current_node->header.prev;
        if (prev_id == 0) {
            iter->valid = false;
            pthread_rwlock_unlock(&iter->tree->tree_lock.lock);
            return false;
        }
        iter->current_node = node_pool_get(&iter->tree->node_pool, prev_id);
        if (iter->current_node) {
            iter->current_index = iter->current_node->header.num_keys - 1;
        }
    }

    pthread_rwlock_unlock(&iter->tree->tree_lock.lock);
    return iter->valid && iter->current_node != NULL;
}

const LeafEntry* btree_iterator_get(BTreeIterator* iter) {
    if (!iter || !iter->valid || !iter->current_node) return NULL;
    if (iter->current_index < 0 ||
        iter->current_index >= iter->current_node->header.num_keys) {
        return NULL;
    }
    return &iter->current_node->entries.leaf[iter->current_index];
}

/* ==================== 范围查询 ==================== */

int btree_range_search(BTree* tree,
                       const uint8_t* start_key, uint16_t start_len,
                       const uint8_t* end_key, uint16_t end_len,
                       int (*callback)(const uint8_t* key, uint16_t key_len,
                                      const uint8_t* value, uint16_t value_len,
                                      void* user_data),
                       void* user_data) {
    if (!tree || !callback) return BTREE_ERR_INVALID;

    BTreeIterator* iter = btree_iterator_create(tree, start_key, start_len,
                                                end_key, end_len, true);
    if (!iter) return BTREE_ERR_NOMEM;

    int count = 0;
    const LeafEntry* entry;

    /* 获取第一个条目 */
    entry = btree_iterator_get(iter);
    while (entry) {
        int result = callback(entry->key.data, entry->key.length,
                             entry->value, entry->value_len, user_data);
        if (result != 0) break;
        count++;

        if (!btree_iterator_next(iter)) break;
        entry = btree_iterator_get(iter);
    }

    btree_iterator_destroy(iter);
    return count;
}
