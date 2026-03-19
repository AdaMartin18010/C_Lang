/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\01_B_Tree_Index.md
 * Line: 1329
 * Language: c
 * Block ID: c689ca98
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ==================== 并发控制实现 ==================== */

/*
 * 乐观锁读取 - 无锁读取模式
 *
 * 原理：
 * 1. 读取前记录版本号
 * 2. 执行读取操作
 * 3. 读取后验证版本号未变化
 * 4. 若版本变化，重试或转为悲观锁
 */
typedef struct {
    BTree* tree;
    BTreeVersion start_version;
    bool active;
} OptimisticRead;

static BTreeVersion btree_get_version(BTree* tree) {
    return atomic_load(&tree->stats.inserts) +
           atomic_load(&tree->stats.deletes);
}

/*
 * 开始乐观读取
 */
static void optimistic_read_begin(OptimisticRead* read, BTree* tree) {
    read->tree = tree;
    read->start_version = btree_get_version(tree);
    read->active = true;
}

/*
 * 验证乐观读取是否有效
 */
static bool optimistic_read_validate(OptimisticRead* read) {
    if (!read->active) return false;
    BTreeVersion current = btree_get_version(read->tree);
    return current == read->start_version;
}

/*
 * 结束乐观读取
 */
static void optimistic_read_end(OptimisticRead* read) {
    read->active = false;
}

/*
 * 使用乐观锁的搜索
 */
int btree_search_optimistic(BTree* tree, const uint8_t* key, uint16_t key_len,
                            uint8_t* value, uint16_t* value_len) {
    if (!tree || !key || key_len == 0) return BTREE_ERR_INVALID;

    OptimisticRead read;
    optimistic_read_begin(&read, tree);

    int max_retries = 3;
    int result;

    for (int retry = 0; retry < max_retries; retry++) {
        /* 执行读取（无锁） */
        SearchPath path;
        result = btree_search_path(tree, key, key_len, &path);

        if (result != BTREE_OK) {
            optimistic_read_end(&read);
            return result;
        }

        /* 复制结果 */
        BTreeNode* leaf = path.nodes[path.depth - 1];
        int idx = path.indices[path.depth - 1];

        /* 在验证前复制数据 */
        uint8_t temp_value[BTREE_VALUE_SIZE];
        uint16_t temp_len = 0;
        bool found = false;

        if (idx >= 0 && leaf) {
            LeafEntry* entry = &leaf->entries.leaf[idx];
            temp_len = entry->value_len;
            memcpy(temp_value, entry->value, temp_len);
            found = true;
        }

        /* 验证版本 */
        if (optimistic_read_validate(&read)) {
            optimistic_read_end(&read);
            if (found) {
                if (value && value_len) {
                    uint16_t copy_len = (temp_len < *value_len) ? temp_len : *value_len;
                    memcpy(value, temp_value, copy_len);
                    *value_len = copy_len;
                }
                return BTREE_OK;
            }
            return BTREE_ERR_NOTFOUND;
        }

        /* 版本冲突，重试 */
        optimistic_read_begin(&read, tree);
    }

    /* 重试耗尽，使用悲观锁 */
    optimistic_read_end(&read);
    return btree_search(tree, key, key_len, value, value_len);
}

/*
 * 读写锁初始化
 */
static int btree_rwlock_init(BTreeRwLock* lock) {
    int result = pthread_rwlock_init(&lock->lock, NULL);
    if (result != 0) return BTREE_ERR_LOCK;

    atomic_init(&lock->readers, 0);
    atomic_init(&lock->writers, 0);
    return BTREE_OK;
}

static void btree_rwlock_destroy(BTreeRwLock* lock) {
    pthread_rwlock_destroy(&lock->lock);
}
