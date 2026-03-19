/*
 * Auto-generated from: 03_System_Technology_Domains\04_Video_Codec\01_H264_Decoding.md
 * Line: 451
 * Language: c
 * Block ID: 2f0f3e11
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 0x000001可能出现在编码数据中，需要转义
// 解决方案：使用emulation_prevention_three_byte

void remove_emulation_prevention(uint8_t *src, int src_len, uint8_t *dst) {
    int j = 0;
    for (int i = 0; i < src_len; i++) {
        if (i >= 2 && src[i-2] == 0x00 && src[i-1] == 0x00 && src[i] == 0x03) {
            // 跳过0x03
            continue;
        }
        dst[j++] = src[i];
    }
}
