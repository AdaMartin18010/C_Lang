/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\03_CBMC_Model_Checking.md
 * Line: 102
 * Language: c
 * Block ID: 6dc3cc78
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 原始代码
int sum_array(int *arr, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];  // 需要展开 n 次
    }
    return sum;
}
