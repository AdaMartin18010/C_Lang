/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\03_CBMC_Model_Checking.md
 * Line: 115
 * Language: c
 * Block ID: bbddb93d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 概念性展开
int sum_array_unrolled(int *arr, int n) {
    int sum = 0;
    int i = 0;

    // 迭代 1
    if (i < n) { sum += arr[i]; i++; }
    // 迭代 2
    if (i < n) { sum += arr[i]; i++; }
    // 迭代 3
    if (i < n) { sum += arr[i]; i++; }

    // 断言：循环已完成
    assert(i >= n);

    return sum;
}
