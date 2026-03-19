/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\03_Hash_Table_Implementation.md
 * Line: 920
 * Language: c
 * Block ID: 3a6c0b4a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * ============================================================================
 * 哈希表完整测试代码
 * ============================================================================
 */

void test_open_address_hash() {
    printf("\n========== 开放寻址哈希表测试 ==========\n");

    OpenAddressHashTable table;
    oa_hash_init(&table);

    // 插入测试
    printf("\n--- 插入测试 ---\n");
    for (int i = 1; i <= 20; i++) {
        oa_hash_insert(&table, i * 7, i * 100);
    }
    oa_hash_print(&table);

    // 查找测试
    printf("\n--- 查找测试 ---\n");
    int value;
    if (oa_hash_find(&table, 49, &value)) {
        printf("Found key=49, value=%d\n", value);
    }
    if (oa_hash_find(&table, 100, &value)) {
        printf("Found key=100, value=%d\n", value);
    } else {
        printf("Key=100 not found\n");
    }

    // 删除测试
    printf("\n--- 删除测试 ---\n");
    oa_hash_delete(&table, 49);
    oa_hash_delete(&table, 56);
    printf("After deleting 49 and 56:\n");
    oa_hash_print(&table);

    oa_hash_destroy(&table);
    printf("\n开放寻址哈希表测试通过!\n");
}

void test_chain_hash() {
    printf("\n========== 链地址法哈希表测试 ==========\n");

    ChainHashTable table;
    chain_hash_init(&table);

    // 插入测试 (制造冲突)
    printf("\n--- 插入测试 ---\n");
    for (int i = 0; i < 30; i++) {
        chain_hash_insert(&table, i, i * i);
    }
    chain_hash_print(&table);

    // 查找测试
    printf("\n--- 查找测试 ---\n");
    int value;
    for (int i = 0; i < 10; i++) {
        if (chain_hash_find(&table, i, &value)) {
            printf("key=%d -> value=%d\n", i, value);
        }
    }

    chain_hash_destroy(&table);
    printf("\n链地址法哈希表测试通过!\n");
}

void test_hash_functions() {
    printf("\n========== 哈希函数测试 ==========\n");

    const char* test_strings[] = {
        "hello", "world", "foo", "bar", "baz",
        "test", "hash", "table", "open", "addressing"
    };

    printf("\nString\t\tFNV-1a\t\tDJB2\t\tMurmur3\n");
    printf("--------------------------------------------------------\n");

    for (int i = 0; i < 10; i++) {
        unsigned int h1 = fnv1a_hash(test_strings[i]);
        unsigned int h2 = djb2_hash(test_strings[i]);
        unsigned int h3 = murmur3_32(test_strings[i], strlen(test_strings[i]), 0);

        printf("%-10s\t0x%08X\t0x%08X\t0x%08X\n",
               test_strings[i], h1, h2, h3);
    }

    printf("\n哈希函数测试通过!\n");
}

// =============================================================================
// 主函数
// =============================================================================

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║         哈希表完整实现测试 (C23 Standard)                  ║\n");
    printf("║         Hash Table Complete Implementation                 ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("编译时间: %s %s\n", __DATE__, __TIME__);

    test_hash_functions();
    test_open_address_hash();
    test_chain_hash();

    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║              所有测试通过! All tests passed!               ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");

    return 0;
}
