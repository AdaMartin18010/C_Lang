/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\01_B_Tree_Index.md
 * Line: 603
 * Language: c
 * Block ID: 3eb81b2f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ==================== 键比较函数 ==================== */

static inline int key_compare(const BTreeKey* a, const BTreeKey* b) {
    uint16_t min_len = (a->length < b->length) ? a->length : b->length;
    int cmp = memcmp(a->data, b->data, min_len);
    if (cmp != 0) return cmp;
    return (a->length < b->length) ? -1 :
           (a->length > b->length) ? 1 : 0;
}

static inline int key_compare_raw(const BTreeKey* key,
                                  const uint8_t* data, uint16_t len) {
    uint16_t min_len = (key->length < len) ? key->length : len;
    int cmp = memcmp(key->data, data, min_len);
    if (cmp != 0) return cmp;
    return (key->length < len) ? -1 : (key->length > len) ? 1 : 0;
}

/* ==================== 二分查找 ==================== */

/*
 * 在节点中查找键的位置
 * 返回值: >=0 表示找到，<0 表示未找到，插入位置为 -返回值-1
 */
static int btree_node_find_key(BTreeNode* node, const uint8_t* key, uint16_t key_len) {
    int left = 0;
    int right = node->header.num_keys - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        BTreeKey* mid_key;

        if (node->header.type == NODE_TYPE_LEAF || node->header.level == 0) {
            mid_key = &node->entries.leaf[mid].key;
        } else {
            mid_key = &node->entries.internal[mid].key;
        }

        int cmp = key_compare_raw(mid_key, key, key_len);

        if (cmp == 0) {
            return mid;
        } else if (cmp < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -left - 1;
}

/* ==================== 搜索路径 ==================== */

/*
 * 搜索路径结构 - 记录从根到叶子的路径
 */
typedef struct {
    BTreeNode* nodes[BTREE_MAX_HEIGHT];
    int        indices[BTREE_MAX_HEIGHT];
    int        depth;
} SearchPath;

/*
 * 从根节点开始搜索，记录路径
 */
static int btree_search_path(BTree* tree, const uint8_t* key, uint16_t key_len,
                             SearchPath* path) {
    BTreeNode* node = tree->root;
    path->depth = 0;

    while (node != NULL && path->depth < BTREE_MAX_HEIGHT) {
        path->nodes[path->depth] = node;

        if (node->header.type == NODE_TYPE_LEAF || node->header.level == 0) {
            /* 到达叶子节点 */
            path->indices[path->depth] = btree_node_find_key(node, key, key_len);
            path->depth++;
            return BTREE_OK;
        }

        /* 内部节点，继续向下 */
        int idx = btree_node_find_key(node, key, key_len);
        if (idx < 0) {
            idx = -idx - 1;
        } else {
            /* 找到精确匹配，移动到右子树 */
            idx++;
        }

        /* 边界检查 */
        if (idx >= node->header.num_keys) {
            idx = node->header.num_keys - 1;
        }

        path->indices[path->depth] = idx;
        path->depth++;

        PageId child_id = node->entries.internal[idx].child;
        node = node_pool_get(&tree->node_pool, child_id);

        if (node == NULL) {
            return BTREE_ERR_CORRUPT;
        }
    }

    return BTREE_OK;
}

/*
 * 公共搜索接口
 */
int btree_search(BTree* tree, const uint8_t* key, uint16_t key_len,
                 uint8_t* value, uint16_t* value_len) {
    if (!tree || !key || key_len == 0 || key_len > BTREE_KEY_SIZE) {
        return BTREE_ERR_INVALID;
    }

    atomic_fetch_add(&tree->stats.searches, 1);

    /* 获取读锁 */
    pthread_rwlock_rdlock(&tree->tree_lock.lock);

    SearchPath path;
    int result = btree_search_path(tree, key, key_len, &path);

    if (result == BTREE_OK) {
        BTreeNode* leaf = path.nodes[path.depth - 1];
        int idx = path.indices[path.depth - 1];

        if (idx >= 0) {
            /* 找到键 */
            LeafEntry* entry = &leaf->entries.leaf[idx];
            if (value && value_len) {
                uint16_t copy_len = (entry->value_len < *value_len) ?
                                    entry->value_len : *value_len;
                memcpy(value, entry->value, copy_len);
                *value_len = copy_len;
            }
        } else {
            result = BTREE_ERR_NOTFOUND;
        }
    }

    pthread_rwlock_unlock(&tree->tree_lock.lock);
    return result;
}
