/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 844
 * Language: c
 * Block ID: dd67e4ae
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// HLS自动检测突发访问模式
void auto_burst_detection(int *src, int *dst, int n) {
    #pragma HLS INTERFACE mode=m_axi port=src
    #pragma HLS INTERFACE mode=m_axi port=dst

    // 模式1：连续访问（自动突发）
    for (int i = 0; i < n; i++) {
        #pragma HLS PIPELINE
        dst[i] = src[i] + 1;  // HLS自动生成长突发
    }
}

// 影响自动突发的因素
void burst_factors(int *src, int *dst, int n) {
    #pragma HLS INTERFACE mode=m_axi port=src
    #pragma HLS INTERFACE mode=m_axi port=dst

    // 因素1：条件访问会中断突发
    for (int i = 0; i < n; i++) {
        #pragma HLS PIPELINE
        if (i % 2 == 0) {  // 条件可能中断突发
            dst[i] = src[i];
        }
    }

    // 因素2：非连续索引
    for (int i = 0; i < n; i++) {
        #pragma HLS PIPELINE
        dst[i] = src[i * 2];  // 步长为2，仍为规则模式
    }

    // 因素3：复杂索引计算
    for (int i = 0; i < n; i++) {
        #pragma HLS PIPELINE
        int idx = i * i;  // 非线性索引，无法突发
        dst[i] = src[idx];
    }
}
