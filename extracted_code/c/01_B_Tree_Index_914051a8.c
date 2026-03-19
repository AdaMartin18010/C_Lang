/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\01_B_Tree_Index.md
 * Line: 1549
 * Language: c
 * Block ID: 914051a8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ==================== 工具函数 ==================== */

void btree_print_stats(BTree* tree) {
    if (!tree) return;

    printf("=== B+树统计信息 ===\n");
    printf("树高度: %u\n", tree->stats.height);
    printf("B+树阶数: %u\n", tree->order);
    printf("最小键数: %u\n", tree->min_keys);
    printf("插入次数: %llu\n",
           (unsigned long long)atomic_load(&tree->stats.inserts));
    printf("删除次数: %llu\n",
           (unsigned long long)atomic_load(&tree->stats.deletes));
    printf("搜索次数: %llu\n",
           (unsigned long long)atomic_load(&tree->stats.searches));
    printf("分裂次数: %llu\n",
           (unsigned long long)atomic_load(&tree->stats.splits));
    printf("合并次数: %llu\n",
           (unsigned long long)atomic_load(&tree->stats.merges));
    printf("节点池容量: %zu\n", tree->node_pool.capacity);
    printf("节点池已用: %zu\n", tree->node_pool.used);
}

/* 递归验证节点 */
static int btree_validate_node(BTree* tree, BTreeNode* node,
                               BTreeKey* min_key, BTreeKey* max_key,
                               int depth, int* leaf_depth) {
    if (!node) return BTREE_OK;

    /* 验证魔数 */
    if (node->header.magic != BTREE_NODE_MAGIC) {
        fprintf(stderr, "错误: 节点魔数无效\n");
        return BTREE_ERR_CORRUPT;
    }

    /* 验证键数量 */
    if (node->header.num_keys > tree->order) {
        fprintf(stderr, "错误: 节点键数量超过阶数\n");
        return BTREE_ERR_CORRUPT;
    }

    if (node->header.type != NODE_TYPE_LEAF &&
        node->header.num_keys < tree->min_keys &&
        node != tree->root) {
        fprintf(stderr, "警告: 内部节点键数低于最小值\n");
    }

    /* 叶子节点特殊检查 */
    if (node->header.type == NODE_TYPE_LEAF || node->header.level == 0) {
        if (*leaf_depth < 0) {
            *leaf_depth = depth;
        } else if (*leaf_depth != depth) {
            fprintf(stderr, "错误: 叶子节点深度不一致\n");
            return BTREE_ERR_CORRUPT;
        }

        /* 验证键顺序 */
        for (int i = 1; i < node->header.num_keys; i++) {
            if (key_compare(&node->entries.leaf[i-1].key,
                           &node->entries.leaf[i].key) >= 0) {
                fprintf(stderr, "错误: 叶子节点键无序\n");
                return BTREE_ERR_CORRUPT;
            }
        }
    } else {
        /* 内部节点，递归验证子节点 */
        for (int i = 0; i < node->header.num_keys; i++) {
            BTreeNode* child = node_pool_get(&tree->node_pool,
                                             node->entries.internal[i].child);
            if (!child) {
                fprintf(stderr, "错误: 无法加载子节点\n");
                return BTREE_ERR_CORRUPT;
            }

            BTreeKey child_min = (i == 0) ? *min_key : node->entries.internal[i-1].key;
            BTreeKey child_max = node->entries.internal[i].key;

            int result = btree_validate_node(tree, child, &child_min, &child_max,
                                            depth + 1, leaf_depth);
            if (result != BTREE_OK) return result;
        }
    }

    return BTREE_OK;
}

int btree_validate(BTree* tree) {
    if (!tree || !tree->root) return BTREE_ERR_INVALID;

    BTreeKey min_key = {0}, max_key = {0};
    max_key.length = BTREE_KEY_SIZE;
    memset(max_key.data, 0xFF, BTREE_KEY_SIZE);

    int leaf_depth = -1;
    return btree_validate_node(tree, tree->root, &min_key, &max_key, 0, &leaf_depth);
}

/* ==================== 主程序 ==================== */

#include <time.h>

/* 生成随机字符串键 */
static void generate_key(uint8_t* key, uint16_t* len, int index) {
    snprintf((char*)key, BTREE_KEY_SIZE, "key_%08d", index);
    *len = (uint16_t)strlen((char*)key);
}

/* 生成随机值 */
static void generate_value(uint8_t* value, uint16_t* len, int index) {
    snprintf((char*)value, BTREE_VALUE_SIZE,
             "value_data_for_key_index_%d_with_some_padding", index);
    *len = (uint16_t)strlen((char*)value);
}

/* 范围查询回调 */
static int range_callback(const uint8_t* key, uint16_t key_len,
                         const uint8_t* value, uint16_t value_len,
                         void* user_data) {
    int* count = (int*)user_data;
    (*count)++;
    if (*count <= 5) {
        printf("  键: %.*s, 值: %.*s\n",
               key_len, key, value_len, value);
    }
    return 0;
}

int main(int argc, char* argv[]) {
    printf("=====================================\n");
    printf("B+树索引实现演示\n");
    printf("=====================================\n\n");

    /* 创建B+树 */
    BTree* tree = btree_create(32);  /* 使用较小阶数便于观察分裂 */
    if (!tree) {
        fprintf(stderr, "创建B+树失败\n");
        return 1;
    }

    printf("1. B+树创建成功\n");
    printf("   阶数: %d\n", tree->order);
    printf("   页大小: %d bytes\n\n", BTREE_PAGE_SIZE);

    /* 插入测试 */
    printf("2. 插入测试 (1000条记录)\n");
    clock_t start = clock();

    int insert_count = 1000;
    for (int i = 0; i < insert_count; i++) {
        uint8_t key[BTREE_KEY_SIZE];
        uint8_t value[BTREE_VALUE_SIZE];
        uint16_t key_len, value_len;

        generate_key(key, &key_len, i);
        generate_value(value, &value_len, i);

        int result = btree_insert(tree, key, key_len, value, value_len);
        if (result != BTREE_OK) {
            fprintf(stderr, "   插入失败 at %d: %s\n", i, btree_error_string(result));
        }
    }

    clock_t end = clock();
    double insert_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("   插入 %d 条记录，耗时: %.3f ms\n", insert_count, insert_time);
    printf("   平均: %.3f μs/次\n\n", insert_time * 1000 / insert_count);

    /* 验证树结构 */
    printf("3. 验证B+树结构\n");
    int validate_result = btree_validate(tree);
    printf("   验证结果: %s\n\n",
           validate_result == BTREE_OK ? "通过" : "失败");

    /* 搜索测试 */
    printf("4. 搜索测试\n");
    start = clock();

    int search_count = 1000;
    int found_count = 0;
    for (int i = 0; i < search_count; i++) {
        uint8_t key[BTREE_KEY_SIZE];
        uint8_t value[BTREE_VALUE_SIZE];
        uint16_t key_len, value_len = BTREE_VALUE_SIZE;

        generate_key(key, &key_len, i);

        int result = btree_search(tree, key, key_len, value, &value_len);
        if (result == BTREE_OK) {
            found_count++;
        }
    }

    end = clock();
    double search_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("   搜索 %d 次，命中 %d 次，耗时: %.3f ms\n",
           search_count, found_count, search_time);
    printf("   平均: %.3f μs/次\n\n", search_time * 1000 / search_count);

    /* 范围查询测试 */
    printf("5. 范围查询测试\n");
    uint8_t start_key[BTREE_KEY_SIZE] = "key_00000100";
    uint8_t end_key[BTREE_KEY_SIZE] = "key_00000200";

    start = clock();
    int range_count = 0;
    int range_result = btree_range_search(tree,
                                          start_key, (uint16_t)strlen((char*)start_key),
                                          end_key, (uint16_t)strlen((char*)end_key),
                                          range_callback, &range_count);
    end = clock();
    double range_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;

    printf("   范围 [%s, %s] 查询结果:\n", start_key, end_key);
    printf("   共 %d 条记录，耗时: %.3f ms\n\n", range_count, range_time);

    /* 迭代器测试 */
    printf("6. 迭代器测试 (前5条)\n");
    BTreeIterator* iter = btree_iterator_create(tree, NULL, 0, NULL, 0, true);
    if (iter) {
        int iter_count = 0;
        const LeafEntry* entry;

        while ((entry = btree_iterator_get(iter)) != NULL && iter_count < 5) {
            printf("   键: %.*s\n", entry->key.length, entry->key.data);
            iter_count++;
            if (!btree_iterator_next(iter)) break;
        }
        btree_iterator_destroy(iter);
    }
    printf("\n");

    /* 更新测试 */
    printf("7. 更新测试\n");
    uint8_t update_key[BTREE_KEY_SIZE];
    uint16_t update_key_len;
    generate_key(update_key, &update_key_len, 100);

    uint8_t new_value[] = "updated_value_data";
    int update_result = btree_update(tree, update_key, update_key_len,
                                     new_value, (uint16_t)sizeof(new_value));
    printf("   更新键 key_00000100: %s\n",
           update_result == BTREE_OK ? "成功" : btree_error_string(update_result));

    /* 验证更新 */
    uint8_t verify_value[BTREE_VALUE_SIZE];
    uint16_t verify_len = BTREE_VALUE_SIZE;
    btree_search(tree, update_key, update_key_len, verify_value, &verify_len);
    printf("   验证值: %.*s\n\n", verify_len, verify_value);

    /* 删除测试 */
    printf("8. 删除测试\n");
    int delete_count = 100;
    int delete_success = 0;

    for (int i = 0; i < delete_count; i++) {
        uint8_t key[BTREE_KEY_SIZE];
        uint16_t key_len;
        generate_key(key, &key_len, i);

        if (btree_delete(tree, key, key_len) == BTREE_OK) {
            delete_success++;
        }
    }
    printf("   删除 %d 条记录，成功 %d 条\n\n", delete_count, delete_success);

    /* 统计信息 */
    printf("9. 统计信息\n");
    btree_print_stats(tree);

    /* 最终验证 */
    printf("\n10. 最终验证\n");
    validate_result = btree_validate(tree);
    printf("    验证结果: %s\n",
           validate_result == BTREE_OK ? "通过" : "失败");

    /* 清理 */
    btree_destroy(tree);

    printf("\n=====================================\n");
    printf("B+树演示完成\n");
    printf("=====================================\n");

    return 0;
}
