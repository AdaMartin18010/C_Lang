/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md
 * Line: 777
 * Language: c
 * Block ID: 09174f16
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 处理时序违例
 */

// 建立时间违例（Setup Violation）
// 原因：组合逻辑延迟过大
// 解决：插入流水线寄存器
void timing_setup_violation(int in[100], int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        // 复杂组合逻辑
        #pragma HLS LATENCY min=2 max=4
        // 强制插入至少2级流水线
        out[i] = very_complex_function(in[i]);
    }
}

// 保持时间违例（Hold Violation）
// 原因：时钟偏斜或路径太短
// 解决：通常由工具自动处理
void timing_hold_violation(int in[100], int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        int val = in[i];
        #pragma HLS RESOURCE variable=val core=Register
        // 显式注册输出
        out[i] = val;
    }
}
