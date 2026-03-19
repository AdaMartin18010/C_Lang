/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
 * Line: 815
 * Language: c
 * Block ID: 097665c4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 泛型编程：_Generic 和宏
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// ========== _Generic 基础 ==========

// 类型选择表达式
#define TYPE_NAME(x) _Generic((x), \
    int: "int", \
    long: "long", \
    float: "float", \
    double: "double", \
    default: "unknown" \
)

// 泛型绝对值
#define GENERIC_ABS(x) _Generic((x), \
    int: abs, \
    long: labs, \
    long long: llabs, \
    float: fabsf, \
    double: fabs \
)(x)

// ========== constexpr + _Generic 组合 ==========

// 泛型常量计算（有限）
constexpr int int_max(int a, int b) { return a > b ? a : b; }
constexpr long long_max(long a, long b) { return a > b ? a : b; }
constexpr float float_max(float a, float b) { return a > b ? a : b; }
constexpr double double_max(double a, double b) { return a > b ? a : b; }

#define MAX(a, b) _Generic((a), \
    int: int_max, \
    long: long_max, \
    float: float_max, \
    double: double_max \
)(a, b)

// ========== 泛型结构体（手动实现） ==========

// 为每种类型创建不同版本
#define DEFINE_VECTOR(T, NAME) \
    typedef struct { \
        T* data; \
        size_t len; \
        size_t cap; \
    } NAME; \
    \
    constexpr size_t NAME##_element_size(void) { \
        return sizeof(T); \
    }

// 实例化
DEFINE_VECTOR(int, IntVector)
DEFINE_VECTOR(double, DoubleVector)

// ========== C23 泛型的局限性 ==========

// ❌ 无法根据类型参数生成不同实现
// ❌ 无法在编译时遍历结构体字段
// ❌ 无法检查类型是否具有某些操作
// ❌ 泛型代码需要大量宏技巧

// 变通：使用回调函数实现泛型算法
typedef int (*CompareFn)(const void*, const void*);

constexpr void swap(void* a, void* b, size_t size) {
    unsigned char* pa = a;
    unsigned char* pb = b;
    for (size_t i = 0; i < size; i++) {
        unsigned char temp = pa[i];
        pa[i] = pb[i];
        pb[i] = temp;
    }
}
