/*
 * Auto-generated from: 00_VERSION_TRACKING\LLVM_22_C23_Update.md
 * Line: 556
 * Language: c
 * Block ID: 54311430
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用 Zvfbfa 的 BFloat16 向量代码
#include <riscv_vector.h>

// BFloat16 矩阵乘法内核
void bf16_matmul_vlen(
    const __bf16 *a,
    const __bf16 *b,
    float *c,
    size_t m, size_t n, size_t k
) {
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            vfloat32m1_t sum = __riscv_vfmv_v_f_f32m1(0.0f, 4);

            for (size_t l = 0; l < k; l += 4) {
                // 加载 BFloat16 并转换为 float
                vuint16m1_t va = __riscv_vle16_v_u16m1(
                    (const uint16_t*)&a[i * k + l], 4
                );
                vfloat32m1_t fa = __riscv_vfwcvt_f_f_v_f32m1(va, 4);

                vuint16m1_t vb = __riscv_vle16_v_u16m1(
                    (const uint16_t*)&b[l * n + j], 4
                );
                vfloat32m1_t fb = __riscv_vfwcvt_f_f_v_f32m1(vb, 4);

                sum = __riscv_vfmacc_vv_f32m1(sum, fa, fb, 4);
            }

            c[i * n + j] = __riscv_vfmv_f_s_f32m1_f32(sum);
        }
    }
}
