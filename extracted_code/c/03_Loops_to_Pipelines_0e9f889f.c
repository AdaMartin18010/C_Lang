/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\03_Loops_to_Pipelines.md
 * Line: 507
 * Language: c
 * Block ID: 0e9f889f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 因素1：资源限制
void resource_limited(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        // 如果不展开，II=1需要多个乘法器
        #pragma HLS PIPELINE II=1
        c[i] = a[i] * b[i] * 3;  // 需要2个乘法器
    }
}

// 解决方案：资源分配或放宽II
void resource_solved(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=2  // 允许资源复用
        #pragma HLS ALLOCATION instances=mul limit=1 operation
        c[i] = a[i] * b[i] * 3;
    }
}

// 因素2：存储器访问限制
void memory_limited(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        // 每个周期需要2读1写，可能需要双端口RAM
        c[i] = a[i] + b[i];
    }
}

// 解决方案：数组分区
void memory_solved(int a[100], int b[100], int c[100]) {
    #pragma HLS ARRAY_PARTITION variable=a complete
    #pragma HLS ARRAY_PARTITION variable=b complete
    #pragma HLS ARRAY_PARTITION variable=c complete

    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        #pragma HLS UNROLL
        c[i] = a[i] + b[i];
    }
}

// 因素3：数据依赖（真依赖/RAW）
void dependency_limited(int a[100], int *sum) {
    int s = 0;
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1  // 无法达到，存在循环间依赖
        s = s + a[i];  // RAW依赖：s的写依赖上一次迭代的读
    }
    *sum = s;
}

// 解决方案：展开+树形归约
void dependency_solved(int a[100], int *sum) {
    int partial[4] = {0};
    #pragma HLS ARRAY_PARTITION variable=partial complete

    for (int i = 0; i < 100; i += 4) {
        #pragma HLS PIPELINE II=1
        #pragma HLS UNROLL factor=4
        partial[0] += a[i+0];
        partial[1] += a[i+1];
        partial[2] += a[i+2];
        partial[3] += a[i+3];
    }

    *sum = partial[0] + partial[1] + partial[2] + partial[3];
}
