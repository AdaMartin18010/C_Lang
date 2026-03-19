/*
 * Auto-generated from: 10_WebAssembly_C\03_WASM_Modules_Memory_Management.md
 * Line: 2306
 * Language: c
 * Block ID: 35f6e7b7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// simd_memory.c
#include <emscripten/emscripten.h>
#include <wasm_simd128.h>

// SIMD 友好的内存布局
EMSCRIPTEN_KEEPALIVE
void simd_add_arrays(float* result, const float* a, const float* b, int n) {
    int i = 0;

    // 每次处理 4 个 float（128 位）
    for (; i + 4 <= n; i += 4) {
        v128_t va = wasm_v128_load(&a[i]);
        v128_t vb = wasm_v128_load(&b[i]);
        v128_t vr = wasm_f32x4_add(va, vb);
        wasm_v128_store(&result[i], vr);
    }

    // 处理剩余元素
    for (; i < n; i++) {
        result[i] = a[i] + b[i];
    }
}

// 对齐的 SIMD 加载
EMSCRIPTEN_KEEPALIVE
void simd_aligned_add(float* result, const float* a, const float* b, int n) {
    // 确保 16 字节对齐
    // 编译时添加 -msimd128

    int i = 0;

    // 处理未对齐的开头
    while (i < n && ((uintptr_t)&a[i] & 15)) {
        result[i] = a[i] + b[i];
        i++;
    }

    // SIMD 处理对齐部分
    int simd_end = n - (n - i) % 4;
    for (; i < simd_end; i += 4) {
        v128_t va = wasm_v128_load(&a[i]);
        v128_t vb = wasm_v128_load(&b[i]);
        v128_t vr = wasm_f32x4_add(va, vb);
        wasm_v128_store(&result[i], vr);
    }

    // 处理剩余元素
    for (; i < n; i++) {
        result[i] = a[i] + b[i];
    }
}
