/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md
 * Line: 47
 * Language: c
 * Block ID: e6b6a2a1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 吞吐率优化目标：每周期处理更多数据
 */

// 策略1：完美流水线（II=1）
void throughput_pipeline(int in[1000], int out[1000]) {
    for (int i = 0; i < 1000; i++) {
        #pragma HLS PIPELINE II=1
        // 每周期启动一次迭代
        out[i] = complex_function(in[i]);
    }
}
// 吞吐率：1样本/周期

// 策略2：向量化处理
void throughput_vector(int in[1000], int out[1000]) {
    for (int i = 0; i < 1000; i += 8) {
        #pragma HLS PIPELINE II=1
        #pragma HLS UNROLL factor=8
        // 每周期处理8个样本
        out[i+0] = process(in[i+0]);
        out[i+1] = process(in[i+1]);
        // ... 重复8次
        out[i+7] = process(in[i+7]);
    }
}
// 吞吐率：8样本/周期

// 策略3：数据流流水线
void throughput_dataflow(int in[1000], int out[1000]) {
    #pragma HLS DATAFLOW

    int temp1[1000], temp2[1000];
    #pragma HLS STREAM variable=temp1 depth=32
    #pragma HLS STREAM variable=temp2 depth=32

    // 三个阶段并发执行
    stage1_read(in, temp1, 1000);
    stage2_process(temp1, temp2, 1000);
    stage3_write(temp2, out, 1000);
}
// 吞吐率：受限于最慢阶段，约1样本/周期

// 策略4：多通道并行
void throughput_multi_channel(
    int ch0[1000], int ch1[1000], int ch2[1000], int ch3[1000],
    int out[1000]
) {
    #pragma HLS ARRAY_PARTITION variable=out cyclic factor=4

    for (int i = 0; i < 1000; i++) {
        #pragma HLS PIPELINE II=1
        #pragma HLS UNROLL factor=4
        // 4通道同时处理
        out[i*4+0] = process(ch0[i]);
        out[i*4+1] = process(ch1[i]);
        out[i*4+2] = process(ch2[i]);
        out[i*4+3] = process(ch3[i]);
    }
}
// 吞吐率：4样本/周期
