/*
 * Auto-generated from: 16_Embedded_AI_Agents\06_C_Frameworks\01_CMSIS_NN.md
 * Line: 482
 * Language: c
 * Block ID: 27dd42d9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 针对Helium矢量扩展优化
#if defined(ARM_MATH_MVEI)

#include "arm_mve.h"

// 使用Helium指令的定制卷积
void conv_helium_optimized(const q7_t* input,
                          const q7_t* weights,
                          q7_t* output,
                          int in_ch, int out_ch, int size) {

    for (int oc = 0; oc < out_ch; oc++) {
        int32_t sum = 0;
        const q7_t* w = weights + oc * in_ch;

        int i = 0;
        // Helium矢量处理 (每次16个int8)
        for (; i <= in_ch - 16; i += 16) {
            int8x16_t vin = vld1q_s8(input + i);
            int8x16_t vw = vld1q_s8(w + i);

            // 矢量乘累加
            int16x8_t prod_low = vmullb_s8(vin, vw);
            int16x8_t prod_high = vmullt_s8(vin, vw);

            sum += vaddvq_s16(prod_low);
            sum += vaddvq_s16(prod_high);
        }

        // 剩余标量处理
        for (; i < in_ch; i++) {
            sum += input[i] * w[i];
        }

        output[oc] = (q7_t)__SSAT((sum >> 7), 8);
    }
}

#endif  // ARM_MATH_MVEI
