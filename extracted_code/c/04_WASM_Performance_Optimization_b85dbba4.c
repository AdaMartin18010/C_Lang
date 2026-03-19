/*
 * Auto-generated from: 10_WebAssembly_C\04_WASM_Performance_Optimization.md
 * Line: 271
 * Language: c
 * Block ID: b85dbba4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// function_optimization.c
#include <emscripten/emscripten.h>

// ========== 强制内联 ==========

// 小函数应该内联
static inline int clamp(int x, int min, int max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

static inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

// 使用 __attribute__((always_inline))
static __attribute__((always_inline)) int min_inline(int a, int b) {
    return a < b ? a : b;
}

// ========== 分支预测提示 ==========

#ifdef __EMSCRIPTEN__
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define LIKELY(x) (x)
#define UNLIKELY(x) (x)
#endif

EMSCRIPTEN_KEEPALIVE
int process_with_hints(int* data, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        // 假设大多数值是正数
        if (LIKELY(data[i] > 0)) {
            sum += data[i];
        } else {
            sum -= data[i];  // 处理负数
        }
    }
    return sum;
}

// ========== 减少函数调用开销 ==========

// 差的：每次迭代都调用函数
float calculate_slow(float x, int n) {
    float result = 0;
    for (int i = 0; i < n; i++) {
        result += sqrtf(x + i);  // 函数调用开销
    }
    return result;
}

// 好的：向量化友好的形式
float calculate_fast(float x, int n) {
    float result = 0;
    float current = x;
    for (int i = 0; i < n; i++) {
        // 使用近似或其他优化
        result += current;  // 简化示例
        current += 1.0f;
    }
    return result;
}

// ========== 尾递归优化 ==========

// 尾递归形式（编译器可能优化为循环）
int factorial_tail(int n, int acc) {
    if (n <= 1) return acc;
    return factorial_tail(n - 1, n * acc);
}

int factorial(int n) {
    return factorial_tail(n, 1);
}
