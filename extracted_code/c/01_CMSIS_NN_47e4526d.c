/*
 * Auto-generated from: 16_Embedded_AI_Agents\06_C_Frameworks\01_CMSIS_NN.md
 * Line: 447
 * Language: c
 * Block ID: 47e4526d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优化: 权重重排以提高缓存命中率
// CMSIS-NN使用特殊的权重布局

// 原始权重布局: [out_ch][h][w][in_ch]
// CMSIS-NN布局: 重排为 [out_ch/4][h][w][in_ch][4] (对于MVE)

// 权重转换函数
void convert_weights_for_cmsis(const q7_t* src_weights,
                              q7_t* dst_weights,
                              uint16_t out_ch,
                              uint16_t h,
                              uint16_t w,
                              uint16_t in_ch) {
    int dst_idx = 0;
    for (int oc = 0; oc < out_ch; oc += 4) {
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                for (int ic = 0; ic < in_ch; ic++) {
                    for (int i = 0; i < 4 && (oc + i) < out_ch; i++) {
                        int src_idx = ((oc + i) * h * w * in_ch) +
                                     (y * w * in_ch) +
                                     (x * in_ch) +
                                     ic;
                        dst_weights[dst_idx++] = src_weights[src_idx];
                    }
                }
            }
        }
    }
}
