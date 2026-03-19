/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md
 * Line: 613
 * Language: c
 * Block ID: 86216a88
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * DATAFLOW减少空闲周期
 */

// 未优化：功能单元空闲时间长
void power_sequential(int in[1000], int out[1000]) {
    int temp[1000];

    // Stage 1运行，Stage 2空闲
    for (int i = 0; i < 1000; i++) {
        #pragma HLS PIPELINE
        temp[i] = stage1(in[i]);
    }

    // Stage 1空闲，Stage 2运行
    for (int i = 0; i < 1000; i++) {
        #pragma HLS PIPELINE
        out[i] = stage2(temp[i]);
    }
}

// 优化：DATAFLOW重叠执行
void power_dataflow(int in[1000], int out[1000]) {
    #pragma HLS DATAFLOW

    hls::stream<int> temp_stream("temp");
    #pragma HLS STREAM variable=temp_stream depth=16

    // 两个stage并行，减少空闲
    stage1_stream(in, temp_stream, 1000);
    stage2_stream(temp_stream, out, 1000);
}
// 更高的资源利用率，单位工作量功耗更低
