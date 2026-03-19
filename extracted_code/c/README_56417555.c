/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\01_Formal_Semantics\README.md
 * Line: 190
 * Language: c
 * Block ID: 56417555
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 计算数组和的程序及其不变式
int sum_array(int arr[], int n) {
    int sum = 0;
    int i = 0;

    // 循环不变式: sum = sum(arr[0..i-1]) 且 0 <= i <= n
    while (i < n) {
        // { sum = sum(arr[0..i-1]) 且 0 <= i < n }
        sum = sum + arr[i];
        // { sum = sum(arr[0..i]) 且 0 <= i < n }
        i = i + 1;
        // { sum = sum(arr[0..i-1]) 且 0 <= i <= n }
    }
    // { sum = sum(arr[0..n-1]) 且 i = n }
    return sum;
}
