/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\01_C_for_Synthesis.md
 * Line: 646
 * Language: c
 * Block ID: 87591f30
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 全局内存 vs 本地存储
void address_spaces(
    int global_mem[1000],  // 外部DRAM
    int *result
) {
    #pragma HLS INTERFACE mode=m_axi port=global_mem

    // 本地存储 - 映射为片上BRAM
    int local_buffer[64];
    #pragma HLS ARRAY_PARTITION variable=local_buffer complete

    // 从全局内存加载到本地
    for (int i = 0; i < 64; i++) {
        #pragma HLS PIPELINE
        local_buffer[i] = global_mem[i];
    }

    // 在本地处理
    int sum = 0;
    for (int i = 0; i < 64; i++) {
        #pragma HLS UNROLL factor=8
        sum += local_buffer[i];
    }

    *result = sum;
}

// 指针算术与地址计算
void pointer_arithmetic(int *base, int offset, int *result) {
    #pragma HLS INTERFACE mode=m_axi port=base

    // 编译器将优化地址计算
    int *ptr = base + offset;  // 地址 = base + offset * sizeof(int)
    *result = *ptr;
}
