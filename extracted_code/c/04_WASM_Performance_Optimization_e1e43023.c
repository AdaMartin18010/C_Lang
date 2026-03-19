/*
 * Auto-generated from: 10_WebAssembly_C\04_WASM_Performance_Optimization.md
 * Line: 359
 * Language: c
 * Block ID: e1e43023
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// simd_basics.c
#include <wasm_simd128.h>
#include <emscripten/emscripten.h>

// ========== SIMD 寄存器类型 ==========
// v128_t - 128 位向量，可以解释为：
// - 16 x i8
// - 8 x i16
// - 4 x i32 / f32
// - 2 x i64 / f64

// ========== 基本 SIMD 操作 ==========

// 加载和存储
EMSCRIPTEN_KEEPALIVE
void simd_load_store_example(const float* input, float* output) {
    // 加载 4 个 float
    v128_t vec = wasm_v128_load(input);

    // 存储 4 个 float
    wasm_v128_store(output, vec);
}

// 标量广播
EMSCRIPTEN_KEEPALIVE
void simd_splat_example(float scalar, float* output) {
    // 将标量复制到所有 4 个位置
    v128_t vec = wasm_f32x4_splat(scalar);
    wasm_v128_store(output, vec);
}

// 构造向量
EMSCRIPTEN_KEEPALIVE
void simd_make_example(float a, float b, float c, float d, float* output) {
    v128_t vec = wasm_f32x4_make(a, b, c, d);
    wasm_v128_store(output, vec);
}

// ========== 算术运算 ==========

EMSCRIPTEN_KEEPALIVE
void simd_arithmetic(const float* a, const float* b, float* sum, float* diff, float* prod) {
    v128_t va = wasm_v128_load(a);
    v128_t vb = wasm_v128_load(b);

    // 加法
    v128_t vsum = wasm_f32x4_add(va, vb);
    wasm_v128_store(sum, vsum);

    // 减法
    v128_t vdiff = wasm_f32x4_sub(va, vb);
    wasm_v128_store(diff, vdiff);

    // 乘法
    v128_t vprod = wasm_f32x4_mul(va, vb);
    wasm_v128_store(prod, vprod);

    // 除法
    // v128_t vdiv = wasm_f32x4_div(va, vb);
}

// 整数 SIMD
EMSCRIPTEN_KEEPALIVE
void simd_integer(const int* a, const int* b, int* result) {
    v128_t va = wasm_v128_load(a);
    v128_t vb = wasm_v128_load(b);

    // 整数加法
    v128_t vadd = wasm_i32x4_add(va, vb);

    // 整数乘法（低半部分）
    v128_t vmul = wasm_i32x4_mul(va, vb);

    // 位与
    v128_t vand = wasm_v128_and(va, vb);

    wasm_v128_store(result, vadd);
    wasm_v128_store(result + 4, vmul);
    wasm_v128_store(result + 8, vand);
}

// ========== 比较操作 ==========

EMSCRIPTEN_KEEPALIVE
void simd_compare(const float* a, const float* b, int* mask) {
    v128_t va = wasm_v128_load(a);
    v128_t vb = wasm_v128_load(b);

    // 比较 a > b
    v128_t vgt = wasm_f32x4_gt(va, vb);

    // 比较 a >= b
    v128_t vge = wasm_f32x4_ge(va, vb);

    // 比较 a == b
    v128_t veq = wasm_f32x4_eq(va, vb);

    // 提取掩码（将 -1 (true) 转为 1）
    v128_t mask_gt = wasm_i32x4_shr(vgt, 31);
    wasm_v128_store(mask, mask_gt);
}

// ========== 洗牌和重排 ==========

EMSCRIPTEN_KEEPALIVE
void simd_shuffle_example(const float* input, float* output) {
    v128_t v = wasm_v128_load(input);

    // 逆序: [3, 2, 1, 0]
    v128_t reversed = wasm_i32x4_shuffle(v, v, 3, 2, 1, 0);
    wasm_v128_store(output, reversed);

    // 广播第一个元素: [0, 0, 0, 0]
    v128_t broadcast = wasm_i32x4_shuffle(v, v, 0, 0, 0, 0);
    wasm_v128_store(output + 4, broadcast);
}
