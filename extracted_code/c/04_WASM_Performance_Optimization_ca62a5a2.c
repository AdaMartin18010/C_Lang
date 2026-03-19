/*
 * Auto-generated from: 10_WebAssembly_C\04_WASM_Performance_Optimization.md
 * Line: 1673
 * Language: c
 * Block ID: ca62a5a2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// comprehensive_benchmark.c
#include <emscripten/emscripten.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// ========== 测试 1: CPU 密集型计算 ==========

EMSCRIPTEN_KEEPALIVE
double benchmark_cpu(int iterations) {
    double sum = 0.0;
    for (int i = 0; i < iterations; i++) {
        sum += sqrt((double)i) * sin((double)i) * cos((double)i);
    }
    return sum;
}

// ========== 测试 2: 内存带宽 ==========

EMSCRIPTEN_KEEPALIVE
float benchmark_memory_read(float* data, int size) {
    float sum = 0.0f;
    for (int i = 0; i < size; i++) {
        sum += data[i];
    }
    return sum;
}

EMSCRIPTEN_KEEPALIVE
void benchmark_memory_write(float* data, int size, float value) {
    for (int i = 0; i < size; i++) {
        data[i] = value;
    }
}

EMSCRIPTEN_KEEPALIVE
void benchmark_memory_copy(float* dst, const float* src, int size) {
    memcpy(dst, src, size * sizeof(float));
}

// ========== 测试 3: 分支预测 ==========

EMSCRIPTEN_KEEPALIVE
int benchmark_branch_predictable(int* data, int size) {
    int sum = 0;
    for (int i = 0; i < size; i++) {
        // 可预测的模式
        if (data[i] < 100) {
            sum += data[i];
        }
    }
    return sum;
}

EMSCRIPTEN_KEEPALIVE
int benchmark_branch_random(int* data, int size) {
    int sum = 0;
    for (int i = 0; i < size; i++) {
        // 随机模式（难以预测）
        if (data[i] & 1) {
            sum += data[i];
        }
    }
    return sum;
}

// ========== 测试 4: 函数调用开销 ==========

static inline int inline_add(int a, int b) {
    return a + b;
}

static int normal_add(int a, int b) {
    return a + b;
}

EMSCRIPTEN_KEEPALIVE
int benchmark_inline_calls(int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum = inline_add(sum, i);
    }
    return sum;
}

EMSCRIPTEN_KEEPALIVE
int benchmark_normal_calls(int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum = normal_add(sum, i);
    }
    return sum;
}

// ========== 测试 5: 递归 vs 迭代 ==========

EMSCRIPTEN_KEEPALIVE
long long benchmark_factorial_recursive(int n) {
    if (n <= 1) return 1;
    return n * benchmark_factorial_recursive(n - 1);
}

EMSCRIPTEN_KEEPALIVE
long long benchmark_factorial_iterative(int n) {
    long long result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

// ========== 测试 6: 浮点运算 ==========

EMSCRIPTEN_KEEPALIVE
float benchmark_float_ops(float* a, float* b, int n) {
    float result = 0.0f;
    for (int i = 0; i < n; i++) {
        result += a[i] * b[i] + sqrtf(fabsf(a[i]));
    }
    return result;
}

EMSCRIPTEN_KEEPALIVE
double benchmark_double_ops(double* a, double* b, int n) {
    double result = 0.0;
    for (int i = 0; i < n; i++) {
        result += a[i] * b[i] + sqrt(fabs(a[i]));
    }
    return result;
}

// ========== 性能报告格式 ==========
/*
完整基准测试报告:

测试项目                      WASM时间    JS时间     加速比
────────────────────────────────────────────────────────────
CPU密集型 (1M迭代)            12.5ms      45.3ms     3.6x
内存读取 (10M float)          8.2ms       15.7ms     1.9x
内存写入 (10M float)          5.1ms       8.9ms      1.7x
内存拷贝 (10M float)          7.8ms       12.3ms     1.6x
分支预测友好                  3.2ms       4.5ms      1.4x
分支预测不友好                8.7ms       9.1ms      1.0x
内联函数调用 (1M次)           1.1ms       2.8ms      2.5x
普通函数调用 (1M次)           2.5ms       3.2ms      1.3x
阶乘递归 (n=20, 100K次)       18.3ms      42.1ms     2.3x
阶乘迭代 (n=20, 100K次)       5.2ms       8.7ms      1.7x
浮点运算 (1M次)               6.7ms       23.4ms     3.5x
双精度运算 (1M次)             8.9ms       28.7ms     3.2x

编译: emcc benchmark.c -O3 -msimd128 -flto
环境: Chrome 120, M1 MacBook Pro
*/
