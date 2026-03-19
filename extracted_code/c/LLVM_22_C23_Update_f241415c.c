/*
 * Auto-generated from: 00_VERSION_TRACKING\LLVM_22_C23_Update.md
 * Line: 414
 * Language: c
 * Block ID: f241415c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// APX 提供的新特性示例
// - 32 个额外的通用寄存器 (R16-R31)
// - 增强的条件执行
// - 新的比较-分支指令

// 受益于 APX 的代码模式
void apx_optimized_loop(int *arr, size_t n) {
    // APX 允许更多寄存器，减少 spill
    int sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;

    for (size_t i = 0; i < n; i += 4) {
        sum1 += arr[i];
        sum2 += arr[i + 1];
        sum3 += arr[i + 2];
        sum4 += arr[i + 3];
    }

    // 使用 CCMP (条件比较) 优化分支
    int total = sum1 + sum2 + sum3 + sum4;
    return total;
}
