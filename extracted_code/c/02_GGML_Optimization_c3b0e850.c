/*
 * Auto-generated from: 16_Embedded_AI_Agents\02_LLMs_on_Edge\02_GGML_Optimization.md
 * Line: 142
 * Language: c
 * Block ID: c3b0e850
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Q4_0量化格式结构
 *
 * 每32个FP32权重压缩为:
 * - 1个FP32缩放因子 (scale)
 * - 16字节量化数据 (32个4-bit值)
 *
 * 压缩比: 32*4字节 / (4+16)字节 = 128/20 = 6.4x
 */

/* 量化过程 */
void quantize_q4_0(const float* src, block_q4_0* dst, int n) {
    const int block_size = 32;  // QK4_0
    const int nb = n / block_size;

    for (int i = 0; i < nb; i++) {
        float amax = 0.0f;

        // 找到绝对值最大值
        for (int j = 0; j < block_size; j++) {
            float v = fabsf(src[i * block_size + j]);
            if (v > amax) amax = v;
        }

        // 计算缩放因子
        const float scale = amax / 7.0f;  // 7 = (1 << 3) - 1
        dst[i].d = scale;

        // 量化
        for (int j = 0; j < block_size / 2; j++) {
            const float x0 = src[i * block_size + 0 + j] / scale;
            const float x1 = src[i * block_size + 16 + j] / scale;

            const uint8_t xi0 = (uint8_t)(x0 + 8.5f);  // +8用于存储符号
            const uint8_t xi1 = (uint8_t)(x1 + 8.5f);

            dst[i].qs[j] = (xi0 & 0x0F) | ((xi1 & 0x0F) << 4);
        }
    }
}

/* 反量化过程 */
void dequantize_q4_0(const block_q4_0* src, float* dst, int n) {
    const int block_size = 32;
    const int nb = n / block_size;

    for (int i = 0; i < nb; i++) {
        const float scale = src[i].d;

        for (int j = 0; j < block_size / 2; j++) {
            const uint8_t vi = src[i].qs[j];

            // 解包4-bit值
            const int8_t vi0 = (vi & 0x0F) - 8;  // -8恢复符号
            const int8_t vi1 = (vi >> 4) - 8;

            dst[i * block_size + 0 + j] = vi0 * scale;
            dst[i * block_size + 16 + j] = vi1 * scale;
        }
    }
}
