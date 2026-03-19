/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
 * Line: 1328
 * Language: c
 * Block ID: 555e5f46
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 静态断言与编译时检查
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

// ========== static_assert ==========

// 基本静态断言
static_assert(sizeof(int) == 4, "int must be 32-bit");
static_assert(alignof(max_align_t) >= 8, "Alignment must be at least 8");

// 与 constexpr 结合
constexpr size_t BUFFER_SIZE = 1024;
static_assert(BUFFER_SIZE % 64 == 0, "Buffer size must be cache line aligned");

// ========== 自定义编译时检查 ==========

// 使用 constexpr 实现条件编译
constexpr bool is_power_of_two(size_t n) {
    return n > 0 && (n & (n - 1)) == 0;
}

#define STATIC_ASSERT_POWER_OF_TWO(n) \
    static_assert(is_power_of_two(n), #n " must be power of two")

STATIC_ASSERT_POWER_OF_TWO(1024);  // OK
// STATIC_ASSERT_POWER_OF_TWO(1000);  // 编译错误

// ========== 类型特征检查 ==========

// 检查类型大小（有限）
#define ASSERT_SAME_SIZE(T1, T2) \
    static_assert(sizeof(T1) == sizeof(T2), #T1 " and " #T2 " must have same size")

ASSERT_SAME_SIZE(int32_t, float);  // 通常 OK（都是 4 字节）

// ========== constexpr 函数前置条件 ==========

// C23 无法在 constexpr 函数内部产生自定义错误信息
// 只能依赖 static_assert 在外部检查

constexpr int safe_divide(int a, int b) {
    // 无法在编译时产生自定义错误
    return b != 0 ? a / b : 0;
}

// 外部检查
constexpr int DIVISOR = 10;
static_assert(DIVISOR != 0, "Divisor cannot be zero");
constexpr int RESULT = safe_divide(100, DIVISOR);

// ========== 条件编译与 constexpr ==========

// 根据 constexpr 值选择代码路径
constexpr bool USE_OPTIMIZED_PATH = true;

int compute(int x) {
    if (USE_OPTIMIZED_PATH) {
        return x * 2;  // 编译器可能优化
    } else {
        return x + x;
    }
}
