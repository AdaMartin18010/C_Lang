/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\README.md
 * Line: 312
 * Language: c
 * Block ID: bd3cd2a9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 原始循环
void sum_original(int* arr, int n, int* result) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];  // 每次迭代依赖上一次 sum
    }
    *result = sum;
}

// 循环展开 ×4: 增加并行性
void sum_unrolled4(int* arr, int n, int* result) {
    int sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    int i = 0;

    // 主循环: 每次处理4个元素
    for (; i + 3 < n; i += 4) {
        sum0 += arr[i];
        sum1 += arr[i+1];  // 与 sum0 并行!
        sum2 += arr[i+2];  // 与 sum0,sum1 并行!
        sum3 += arr[i+3];  // 与 sum0,sum1,sum2 并行!
    }

    // 剩余元素
    for (; i < n; i++) sum0 += arr[i];

    *result = sum0 + sum1 + sum2 + sum3;
}
