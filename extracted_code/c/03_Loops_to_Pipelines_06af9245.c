/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\03_Loops_to_Pipelines.md
 * Line: 382
 * Language: c
 * Block ID: 06af9245
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 策略1：仅内层流水线
void inner_pipeline(int a[10][10], int b[10][10], int c[10][10]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            #pragma HLS PIPELINE II=1
            c[i][j] = a[i][j] + b[i][j];
        }
    }
}
// 性能：外层串行，内层流水线

// 策略2：完美嵌套循环展平
void flattened_pipeline(int a[10][10], int b[10][10], int c[10][10]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            #pragma HLS PIPELINE II=1
            c[i][j] = a[i][j] + b[i][j];
        }
    }
}

// 策略3：DATAFLOW优化（函数级并行）
void dataflow_pipeline(int a[10][10], int b[10][10], int c[10][10]) {
    #pragma HLS DATAFLOW

    int buf1[10][10], buf2[10][10];

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            #pragma HLS PIPELINE
            buf1[i][j] = a[i][j] * 2;
        }
    }

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            #pragma HLS PIPELINE
            buf2[i][j] = buf1[i][j] + b[i][j];
        }
    }

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            #pragma HLS PIPELINE
            c[i][j] = buf2[i][j] - 1;
        }
    }
}

// 策略4：循环变换优化
void optimized_nested(int a[10][10], int b[10][10], int c[10][10]) {
    #pragma HLS ARRAY_PARTITION variable=a complete dim=2
    #pragma HLS ARRAY_PARTITION variable=b complete dim=2
    #pragma HLS ARRAY_PARTITION variable=c complete dim=2

    for (int i = 0; i < 10; i++) {
        #pragma HLS PIPELINE II=1
        for (int j = 0; j < 10; j++) {
            #pragma HLS UNROLL
            c[i][j] = a[i][j] + b[i][j];
        }
    }
}
