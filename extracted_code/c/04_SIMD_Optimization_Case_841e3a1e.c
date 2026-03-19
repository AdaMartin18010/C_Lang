/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\04_SIMD_Optimization_Case.md
 * Line: 237
 * Language: c
 * Block ID: 841e3a1e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// arm_neon.h - ARM NEON头文件
#include <arm_neon.h>

// NEON 128-bit寄存器类型
// 向量类型
float32x4_t  - 4个float32
float64x2_t  - 2个float64
int32x4_t    - 4个int32
int16x8_t    - 8个int16
int8x16_t    - 16个int8

// 数组类型 (用于存储)
float32x4x4_t - 4个float32x4_t

// 加载操作
float32x4_t vld1q_f32(const float32_t* ptr);     // 加载4个float
float32x4_t vld1q_dup_f32(const float32_t* ptr); // 复制1个到全部

// 存储操作
void vst1q_f32(float32_t* ptr, float32x4_t val);

// 广播
float32x4_t vdupq_n_f32(float32_t val);

// 示例：向量加法
void vec_add_neon(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;

    // NEON每次处理4个float
    for (; i + 4 <= n; i += 4) {
        float32x4_t va = vld1q_f32(&a[i]);
        float32x4_t vb = vld1q_f32(&b[i]);
        float32x4_t vc = vaddq_f32(va, vb);
        vst1q_f32(&c[i], vc);
    }

    // 处理剩余元素
    for (; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}
