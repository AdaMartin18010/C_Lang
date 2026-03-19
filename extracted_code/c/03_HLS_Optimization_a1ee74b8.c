/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md
 * Line: 522
 * Language: c
 * Block ID: a1ee74b8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 减少不必要的时钟翻转
 */

// 策略：条件执行降低活动因子
void power_clock_gating(int in[100], int out[100], int enable) {
    #pragma HLS INTERFACE mode=ap_vld port=enable

    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        if (enable) {  // 条件执行
            out[i] = heavy_computation(in[i]);
        } else {
            out[i] = 0;  // 或保持前值
        }
    }
}
// 当enable=0时，大部分逻辑不翻转

// 策略：门控时钟域
void power_multi_clock(int fast_data[100], int slow_data[10]) {
    #pragma HLS INTERFACE mode=ap_vld port=fast_data
    #pragma HLS INTERFACE mode=ap_vld port=slow_data clock=clk_slow

    // 快速域处理
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        process_fast(fast_data[i]);
    }

    // 慢速域处理（较低时钟频率）
    for (int i = 0; i < 10; i++) {
        #pragma HLS PIPELINE
        process_slow(slow_data[i]);
    }
}
