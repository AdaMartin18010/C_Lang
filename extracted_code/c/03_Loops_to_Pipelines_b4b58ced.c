/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\03_Loops_to_Pipelines.md
 * Line: 450
 * Language: c
 * Block ID: b4b58ced
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// II (Initiation Interval) - 启动间隔
void pipeline_ii(int in[100], int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=2  // 每2周期启动一次迭代
        out[i] = in[i] * 3;
    }
}
// 资源减半，吞吐减半

// rewind - 循环间流水线
void pipeline_rewind(int in[100], int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE rewind
        out[i] = complex_function(in[i]);
    }
}
// 当前循环结束时启动下一次循环

// off - 禁用流水线
void no_pipeline(int in[100], int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE off
        out[i] = in[i] * 3;
    }
}
// 完全顺序执行
