/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\04_SIMD_Optimization_Case.md
 * Line: 283
 * Language: c
 * Block ID: 20499bd6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 乘加运算 (FMA)
float32x4_t vfmaq_f32(float32x4_t a, float32x4_t b, float32x4_t c);
// 结果 = a + b * c

// 乘减运算
float32x4_t vfmsq_f32(float32x4_t a, float32x4_t b, float32x4_t c);
// 结果 = a - b * c

// 水平加法
float32x2_t vpadd_f32(float32x2_t a, float32x2_t b);

// 扩展和窄化
int32x4_t vmovl_s16(int16x4_t a);     // 扩展 int16 -> int32
int16x4_t vqmovn_s32(int32x4_t a);    // 饱和窄化 int32 -> int16

// 条件选择
float32x4_t vbslq_f32(uint32x4_t mask,
                      float32x4_t a,
                      float32x4_t b);
// mask为true的元素选择a，否则选择b

// 查表 (ARMv8特有)
uint8x16_t vqtbl1q_u8(uint8x16_t t, uint8x16_t idx);

// 矩阵乘法优化 (NEON)
void matmul_neon_4x4(const float* A, const float* B, float* C) {
    // 加载C的4行
    float32x4_t c0 = vld1q_f32(&C[0]);
    float32x4_t c1 = vld1q_f32(&C[4]);
    float32x4_t c2 = vld1q_f32(&C[8]);
    float32x4_t c3 = vld1q_f32(&C[12]);

    for (int k = 0; k < 4; k++) {
        // 广播A的列元素
        float32x4_t a0 = vdupq_n_f32(A[0*4 + k]);
        float32x4_t a1 = vdupq_n_f32(A[1*4 + k]);
        float32x4_t a2 = vdupq_n_f32(A[2*4 + k]);
        float32x4_t a3 = vdupq_n_f32(A[3*4 + k]);

        // 加载B的行
        float32x4_t b = vld1q_f32(&B[k*4]);

        // FMA累加
        c0 = vfmaq_f32(c0, a0, b);
        c1 = vfmaq_f32(c1, a1, b);
        c2 = vfmaq_f32(c2, a2, b);
        c3 = vfmaq_f32(c3, a3, b);
    }

    // 存储结果
    vst1q_f32(&C[0], c0);
    vst1q_f32(&C[4], c1);
    vst1q_f32(&C[8], c2);
    vst1q_f32(&C[12], c3);
}
