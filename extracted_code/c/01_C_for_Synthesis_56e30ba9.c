/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\01_C_for_Synthesis.md
 * Line: 767
 * Language: c
 * Block ID: 56e30ba9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 原则1：最大化并行性
void maximize_parallelism(int a[100], int b[100], int c[100]) {
    #pragma HLS ARRAY_PARTITION variable=a complete
    #pragma HLS ARRAY_PARTITION variable=b complete
    #pragma HLS ARRAY_PARTITION variable=c complete

    for (int i = 0; i < 100; i++) {
        #pragma HLS UNROLL
        c[i] = a[i] * b[i];
    }
}

// 原则2：数据流优化
void dataflow_optimization(
    int in[100],
    int out[100]
) {
    #pragma HLS DATAFLOW

    int temp1[100], temp2[100];

    // 三个阶段并行执行（流水线方式）
    stage1(in, temp1);
    stage2(temp1, temp2);
    stage3(temp2, out);
}

// 原则3：循环优化
void loop_optimization(int a[1000]) {
    // 合并小循环
    for (int i = 0; i < 1000; i++) {
        #pragma HLS PIPELINE
        a[i] = process1(a[i]);
        a[i] = process2(a[i]);
    }

    // 循环展开
    for (int i = 0; i < 100; i += 4) {
        #pragma HLS PIPELINE
        #pragma HLS UNROLL factor=4
        a[i+0] = func(a[i+0]);
        a[i+1] = func(a[i+1]);
        a[i+2] = func(a[i+2]);
        a[i+3] = func(a[i+3]);
    }
}
