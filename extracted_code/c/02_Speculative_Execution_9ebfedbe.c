/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\02_Speculative_Execution.md
 * Line: 91
 * Language: c
 * Block ID: 9ebfedbe
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 分支预测对性能的影响
#include <time.h>

void predictable_branch(int *arr, int n) {
    // 分支预测友好：模式固定
    for (int i = 0; i < n; i++) {
        if (arr[i] < 128) {  // 总是成立或总是不成立
            arr[i] = 0;
        }
    }
}

void unpredictable_branch(int *arr, int n) {
    // 分支预测不友好：随机模式
    for (int i = 0; i < n; i++) {
        if (arr[i] < rand() % 256) {  // 随机分支
            arr[i] = 0;
        }
    }
}

// 性能对比：可预测分支通常快5-10倍
