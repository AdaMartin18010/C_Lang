/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\03_Loops_to_Pipelines.md
 * Line: 679
 * Language: c
 * Block ID: 20a4583f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 解决方案1：依赖指示
void explicit_dep_pragma(int a[100], int *result) {
    int acc = 0;
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        #pragma HLS DEPENDENCE variable=acc inter false
        // 告诉编译器acc在迭代间无依赖（如果不正确会导致错误结果）
        acc = acc + a[i];
    }
    *result = acc;
}

// 解决方案2：循环变换（循环展开归约）
void reduction_tree(int a[100], int *result) {
    // 第一层：并行归约
    int level1[10];
    #pragma HLS ARRAY_PARTITION variable=level1 complete

    for (int i = 0; i < 10; i++) {
        #pragma HLS PIPELINE
        level1[i] = 0;
        for (int j = 0; j < 10; j++) {
            #pragma HLS UNROLL
            level1[i] += a[i * 10 + j];
        }
    }

    // 第二层：求和
    int sum = 0;
    for (int i = 0; i < 10; i++) {
        #pragma HLS UNROLL
        sum += level1[i];
    }

    *result = sum;
}

// 解决方案3：标量替换消除依赖
void scalar_replacement(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        // 使用局部变量消除存储器依赖
        int ai = a[i];  // 只读一次
        int bi = b[i];  // 只读一次
        int ci = ai * ai + bi * bi;  // 计算
        c[i] = ci;      // 只写一次
    }
}

// 解决方案4：循环分块
void loop_tiling(int a[1000], int *result) {
    int sum = 0;
    #define TILE_SIZE 10

    for (int tile = 0; tile < 100; tile++) {
        int tile_sum = 0;

        // 分块内并行归约
        for (int i = 0; i < TILE_SIZE; i++) {
            #pragma HLS PIPELINE
            #pragma HLS UNROLL
            tile_sum += a[tile * TILE_SIZE + i];
        }

        // 块间串行累加
        sum += tile_sum;
    }

    *result = sum;
}
