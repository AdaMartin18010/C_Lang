/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 599
 * Language: c
 * Block ID: 3c7ecca5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 最理想的访问模式：顺序突发
void sequential_access(int *src, int *dst, int n) {
    #pragma HLS INTERFACE mode=m_axi port=src
    #pragma HLS INTERFACE mode=m_axi port=dst

    for (int i = 0; i < n; i++) {
        #pragma HLS PIPELINE II=1
        dst[i] = src[i] * 2;
    }
}
// HLS工具会自动识别为顺序访问，生成突发传输

// 显式突发传输优化
void explicit_burst(int *src, int *dst, int n) {
    #pragma HLS INTERFACE mode=m_axi port=src max_read_burst_length=64
    #pragma HLS INTERFACE mode=m_axi port=dst max_write_burst_length=64

    // 使用局部存储缓冲突发数据
    int burst_buf[64];

    for (int base = 0; base < n; base += 64) {
        // 读突发
        for (int i = 0; i < 64; i++) {
            #pragma HLS PIPELINE
            burst_buf[i] = src[base + i];
        }

        // 处理
        for (int i = 0; i < 64; i++) {
            #pragma HLS PIPELINE
            burst_buf[i] = burst_buf[i] * 2;
        }

        // 写突发
        for (int i = 0; i < 64; i++) {
            #pragma HLS PIPELINE
            dst[base + i] = burst_buf[i];
        }
    }
}
