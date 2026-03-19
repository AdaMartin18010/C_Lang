/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md
 * Line: 113
 * Language: c
 * Block ID: c1bdb684
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 延迟优化目标：从输入到输出的总周期数最小
 */

// 策略1：完全展开消除循环开销
void latency_full_unroll(int a[8], int b[8], int *sum) {
    int s = 0;
    for (int i = 0; i < 8; i++) {
        #pragma HLS UNROLL
        s += a[i] * b[i];
    }
    *sum = s;
}
// 延迟：1-2周期（组合逻辑或短流水线）

// 策略2：树形归约减少加法延迟
void latency_tree_reduction(int a[1024], int *sum) {
    #pragma HLS ARRAY_PARTITION variable=a complete

    // 第一层：512个并行加法
    int level1[512];
    #pragma HLS ARRAY_PARTITION variable=level1 complete
    for (int i = 0; i < 512; i++) {
        #pragma HLS UNROLL
        level1[i] = a[2*i] + a[2*i+1];
    }

    // 第二层：256个并行加法
    int level2[256];
    #pragma HLS ARRAY_PARTITION variable=level2 complete
    for (int i = 0; i < 256; i++) {
        #pragma HLS UNROLL
        level2[i] = level1[2*i] + level1[2*i+1];
    }

    // ... 继续直到只剩一个元素
    // 共10层（log2(1024)）

    *sum = /* 最终结果 */;
}
// 延迟：10周期（树形深度）

// 策略3：内联消除函数调用开销
void latency_inline(int in[100], int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        #pragma HLS INLINE
        // 子函数展开为内联代码
        out[i] = sub_function(in[i]);
    }
}

// 策略4：预加载减少存储器延迟
void latency_prefetch(int global_mem[10000], int result[100]) {
    int local_buf[100];
    #pragma HLS ARRAY_PARTITION variable=local_buf complete

    // 预加载阶段
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        local_buf[i] = global_mem[i];
    }

    // 快速处理阶段
    for (int i = 0; i < 100; i++) {
        #pragma HLS UNROLL
        result[i] = process(local_buf[i]);
    }
}
