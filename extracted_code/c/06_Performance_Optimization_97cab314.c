/*
 * Auto-generated from: 12_Practice_Exercises\06_Performance_Optimization.md
 * Line: 192
 * Language: c
 * Block ID: 97cab314
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 分支多的实现
int sum_positive_branchy(int *arr, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        if (arr[i] > 0) {  // 分支预测可能失败
            sum += arr[i];
        }
    }
    return sum;
}

// 分支少的实现 (使用条件移动)
int sum_positive_branchless(int *arr, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        int mask = arr[i] >> 31;  // 负数时为-1，正数时为0
        sum += arr[i] & ~mask;     // 负数贡献0
    }
    return sum;
}
