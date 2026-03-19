/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md
 * Line: 411
 * Language: c
 * Block ID: c5039511
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 优化存储器使用，减少BRAM消耗
 */

// 问题：使用BRAM存储小数组
void memory_bram_waste(int in[64], int out[64]) {
    int buffer[64];
    #pragma HLS RESOURCE variable=buffer core=RAM_1P

    for (int i = 0; i < 64; i++) {
        #pragma HLS PIPELINE
        buffer[i] = in[i];
    }
    for (int i = 0; i < 64; i++) {
        #pragma HLS PIPELINE
        out[i] = buffer[i];
    }
}
// 使用1个BRAM（18K），利用率低

// 优化1：完全分区使用LUT
void memory_lut_based(int in[64], int out[64]) {
    int buffer[64];
    #pragma HLS ARRAY_PARTITION variable=buffer complete
    // 映射为LUT/FF，不使用BRAM

    for (int i = 0; i < 64; i++) {
        #pragma HLS UNROLL
        buffer[i] = in[i];
    }
    for (int i = 0; i < 64; i++) {
        #pragma HLS UNROLL
        out[i] = buffer[i];
    }
}
// BRAM: 0, LUT: 增加

// 优化2：数组映射合并
void memory_array_map(int a[512], int b[512], int c[512]) {
    #pragma HLS ARRAY_MAP variable=a instance=merged horizontal
    #pragma HLS ARRAY_MAP variable=b instance=merged horizontal
    #pragma HLS ARRAY_MAP variable=c instance=merged horizontal
    // 三个数组合并到一个BRAM中

    for (int i = 0; i < 512; i++) {
        #pragma HLS PIPELINE
        c[i] = a[i] + b[i];
    }
}
// BRAM: 从3减少到1

// 优化3：使用URAM（UltraRAM）替代BRAM
#ifdef URAM
void memory_uram(int large_buffer[8192]) {
    #pragma HLS RESOURCE variable=large_buffer core=URAM
    // 使用UltraRAM，单块容量更大
}
#endif
