/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 688
 * Language: c
 * Block ID: 0277912e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define ROWS 64
#define COLS 64

// 行优先访问 - 缓存友好
void row_major_access(int matrix[ROWS][COLS], int *result) {
    #pragma HLS INTERFACE mode=m_axi port=matrix

    int sum = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            #pragma HLS PIPELINE
            sum += matrix[i][j];  // 连续地址访问
        }
    }
    *result = sum;
}

// 列优先访问 - 需要转置优化
void column_major_access(int matrix[ROWS][COLS], int *result) {
    #pragma HLS INTERFACE mode=m_axi port=matrix

    // 方法1：使用局部缓冲区转置
    int col_buf[ROWS];
    int sum = 0;

    for (int j = 0; j < COLS; j++) {
        // 一次性读取整列到缓冲区
        for (int i = 0; i < ROWS; i++) {
            #pragma HLS PIPELINE
            col_buf[i] = matrix[i][j];
        }

        // 处理缓冲区（顺序访问）
        for (int i = 0; i < ROWS; i++) {
            #pragma HLS PIPELINE
            sum += col_buf[i];
        }
    }
    *result = sum;
}

// 方法2：数组分区实现并行列访问
void parallel_column_access(
    int matrix[ROWS][COLS],
    int *result
) {
    #pragma HLS INTERFACE mode=m_axi port=matrix
    #pragma HLS ARRAY_PARTITION variable=matrix dim=2 complete

    int sum = 0;
    for (int i = 0; i < ROWS; i++) {
        #pragma HLS PIPELINE
        int row_sum = 0;
        for (int j = 0; j < COLS; j++) {
            #pragma HLS UNROLL
            row_sum += matrix[i][j];
        }
        sum += row_sum;
    }
    *result = sum;
}
