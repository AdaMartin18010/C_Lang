/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
 * Line: 533
 * Language: c
 * Block ID: a795e4b4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 constexpr 函数详解
#include <stdio.h>
#include <stdint.h>
#include <limits.h>

// ========== 基本 constexpr 函数 ==========

// 纯计算函数
constexpr int add(int a, int b) {
    return a + b;
}

// 多语句函数（C23 新特性）
constexpr int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// 递归函数
constexpr int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

// ========== constexpr 与类型特征 ==========

// 编译时类型大小计算
constexpr size_t padding_needed(size_t size, size_t align) {
    return (align - (size % align)) % align;
}

constexpr size_t aligned_size(size_t size, size_t align) {
    return size + padding_needed(size, align);
}

// ========== constexpr 使用模式 ==========

// 1. 编译时常量
constexpr int TABLE_SIZE = factorial(5);  // 120

// 2. 数组大小
int lookup_table[TABLE_SIZE];

// 3. switch case 标签
constexpr int ERROR_CODE = 42;

void handle_error(int code) {
    switch (code) {
        case ERROR_CODE:  // 合法：ERROR_CODE 是常量表达式
            printf("Error 42\n");
            break;
        default:
            break;
    }
}

// 4. 位掩码生成
constexpr uint32_t make_mask(int bits) {
    return bits >= 32 ? UINT32_MAX : (1U << bits) - 1;
}

constexpr uint32_t LOW_8 = make_mask(8);    // 0xFF
constexpr uint32_t LOW_16 = make_mask(16);  // 0xFFFF

// ========== constexpr 限制 ==========

// ❌ 错误：动态内存分配
// constexpr int* bad_alloc() {
//     return malloc(sizeof(int));  // 错误：不允许
// }

// ❌ 错误：I/O 操作
// constexpr void bad_print() {
//     printf("Hello\n");  // 错误：不允许 I/O
// }

// ❌ 错误：外部函数调用
// constexpr int bad_call() {
//     return external_func();  // 错误：必须是 constexpr 函数
// }

// ✅ 允许：运行时调用 constexpr 函数
int runtime_call(int x) {
    return factorial(x);  // OK：运行时计算
}
