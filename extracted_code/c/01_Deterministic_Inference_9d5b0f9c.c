/*
 * Auto-generated from: 16_Embedded_AI_Agents\08_Real_Time_AI\01_Deterministic_Inference.md
 * Line: 89
 * Language: c
 * Block ID: 9d5b0f9c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 条件分支导致执行时间不确定
void relu_branch(float* data, int size) {
    for (int i = 0; i < size; i++) {
        if (data[i] < 0) {       // 分支预测可能失败
            data[i] = 0;
        }
    }
}

// ✅ 无分支实现，恒定执行时间
void relu_branchless(float* data, int size) {
    for (int i = 0; i < size; i++) {
        // 使用条件移动，无分支
        data[i] = (data[i] > 0) ? data[i] : 0;
        // 或位运算技巧
        // int32_t mask = ~((int32_t)data[i] >> 31);
        // data[i] = data[i] * mask;
    }
}

// ✅ SIMD无分支实现 (ARM NEON)
#include "arm_neon.h"

void relu_neon(float32_t* data, int size) {
    float32x4_t zero = vdupq_n_f32(0.0f);

    int i = 0;
    for (; i <= size - 4; i += 4) {
        float32x4_t vec = vld1q_f32(&data[i]);
        vec = vmaxq_f32(vec, zero);  // 无分支最大值
        vst1q_f32(&data[i], vec);
    }

    // 剩余元素
    for (; i < size; i++) {
        data[i] = (data[i] > 0) ? data[i] : 0;
    }
}
