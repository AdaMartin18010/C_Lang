/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\01_C_for_Synthesis.md
 * Line: 686
 * Language: c
 * Block ID: f439398a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 数组分区消除存储器瓶颈
void array_partition_example(int a[100], int b[100], int c[100]) {
    #pragma HLS ARRAY_PARTITION variable=a cyclic factor=4
    #pragma HLS ARRAY_PARTITION variable=b block factor=4
    #pragma HLS ARRAY_PARTITION variable=c complete

    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        c[i] = a[i] + b[i];
    }
}

/*
分区策略对比：
┌─────────────┬─────────────────────────────────────────┐
│ 策略        │ 描述                                    │
├─────────────┼─────────────────────────────────────────┤
│ complete    │ 完全分区，每个元素独立端口（最快，最大）│
│ block       │ 块分区，连续元素分到不同存储器          │
│ cyclic      │ 循环分区，交错分配（适合并行访问）      │
└─────────────┴─────────────────────────────────────────┘
*/

// 多维数组分区
typedef int matrix_t[8][8];
void matrix_partition(matrix_t a, matrix_t b, matrix_t c) {
    #pragma HLS ARRAY_PARTITION variable=a dim=2 complete
    #pragma HLS ARRAY_PARTITION variable=b dim=1 complete

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            #pragma HLS PIPELINE
            int sum = 0;
            for (int k = 0; k < 8; k++) {
                sum += a[i][k] * b[k][j];
            }
            c[i][j] = sum;
        }
    }
}
