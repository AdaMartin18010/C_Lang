/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\01_C_for_Synthesis.md
 * Line: 818
 * Language: c
 * Block ID: b482782e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 策略1：操作符共享（通过限制并行度）
void resource_sharing(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=4  // 4周期一次迭代，允许共享乘法器
        c[i] = a[i] * b[i];  // 只需要1个乘法器，时序复用
    }
}

// 策略2：定点数替代浮点数
void use_fixed_point(ap_fixed<16,8> in[100], ap_fixed<16,8> out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        // 定点乘法资源远小于浮点
        out[i] = in[i] * ap_fixed<16,8>(1.5);
    }
}

// 策略3：常量传播
void constant_propagation(int in[100], int out[100]) {
    const int SCALE = 8;  // 2的幂次方可用移位

    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        out[i] = in[i] * SCALE;  // 综合为移位操作，无乘法器
    }
}

// 策略4：存储器优化
void memory_optimization(
    int large_mem[10000],
    int *result
) {
    // 使用小缓冲区减少存储器访问
    int buffer[64];
    #pragma HLS ARRAY_PARTITION variable=buffer complete

    int total = 0;
    for (int base = 0; base < 10000; base += 64) {
        // 加载阶段
        for (int i = 0; i < 64; i++) {
            #pragma HLS PIPELINE
            buffer[i] = large_mem[base + i];
        }
        // 计算阶段
        for (int i = 0; i < 64; i++) {
            #pragma HLS PIPELINE
            total += buffer[i];
        }
    }
    *result = total;
}
