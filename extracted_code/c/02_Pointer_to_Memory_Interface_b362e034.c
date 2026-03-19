/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 990
 * Language: c
 * Block ID: b362e034
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 处理非对齐的突发边界
void aligned_burst_processing(int *src, int *dst, int n) {
    #pragma HLS INTERFACE mode=m_axi port=src
    #pragma HLS INTERFACE mode=m_axi port=dst

    #define BURST_ALIGN 64

    int i = 0;

    // 处理头部非对齐部分
    int head_align = BURST_ALIGN - ((intptr_t)src % BURST_ALIGN);
    for (; i < head_align && i < n; i++) {
        #pragma HLS PIPELINE
        dst[i] = src[i] * 2;
    }

    // 处理中间对齐部分（突发传输）
    int aligned_len = (n - i) / BURST_ALIGN * BURST_ALIGN;
    for (; i < head_align + aligned_len; i += BURST_ALIGN) {
        int local_buf[BURST_ALIGN];

        // 突发读取
        for (int j = 0; j < BURST_ALIGN; j++) {
            #pragma HLS PIPELINE
            local_buf[j] = src[i + j];
        }

        // 处理
        for (int j = 0; j < BURST_ALIGN; j++) {
            #pragma HLS UNROLL factor=8
            local_buf[j] = local_buf[j] * 2;
        }

        // 突发写入
        for (int j = 0; j < BURST_ALIGN; j++) {
            #pragma HLS PIPELINE
            dst[i + j] = local_buf[j];
        }
    }

    // 处理尾部非对齐部分
    for (; i < n; i++) {
        #pragma HLS PIPELINE
        dst[i] = src[i] * 2;
    }
}
