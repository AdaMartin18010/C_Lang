/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\01_C_for_Synthesis.md
 * Line: 106
 * Language: c
 * Block ID: dacebe1e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ✅ 好的可综合代码：关注算法
void fir_filter(int *y, int *x, int *coeff, int len) {
    int acc;
    for (int n = 0; n < len; n++) {
        acc = 0;
        for (int k = 0; k < TAP; k++) {
            acc += x[n - k] * coeff[k];
        }
        y[n] = acc;
    }
}

// ❌ 不好的代码：过于具体的硬件细节
void fir_hardware(int *y, int *x, int *coeff, int len) {
    // 手动管理硬件资源，HLS工具难以优化
    static int shift_reg[16];  // 显式移位寄存器
    #pragma HLS array_partition variable=shift_reg complete
    // ... 复杂的时序控制
}
