/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\03_Loops_to_Pipelines.md
 * Line: 578
 * Language: c
 * Block ID: 46fe8753
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 技术1：循环展开提高II
void unroll_for_ii(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i += 4) {
        #pragma HLS PIPELINE II=1
        #pragma HLS UNROLL factor=4
        c[i+0] = a[i+0] * b[i+0];
        c[i+1] = a[i+1] * b[i+1];
        c[i+2] = a[i+2] * b[i+2];
        c[i+3] = a[i+3] * b[i+3];
    }
}

// 技术2：操作符调度优化
void schedule_optimization(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        int temp1 = a[i] * 3;  // 乘法
        int temp2 = b[i] + 5;  // 加法（无依赖，可并行）
        c[i] = temp1 + temp2;  // 加法
    }
}

// 技术3：存储器bank优化
void bank_optimization(int a[100][4], int b[100]) {
    #pragma HLS ARRAY_PARTITION variable=a dim=2 factor=4

    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        #pragma HLS UNROLL factor=4
        for (int j = 0; j < 4; j++) {
            b[i] += a[i][j];
        }
    }
}

// 技术4：数据打包减少访问
void data_packing(ap_uint<128> *packed, int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        ap_uint<128> p = packed[i];
        // 解包4个32位整数
        int a = p.range(31, 0);
        int b = p.range(63, 32);
        int c = p.range(95, 64);
        int d = p.range(127, 96);
        out[i] = a + b + c + d;
    }
}
