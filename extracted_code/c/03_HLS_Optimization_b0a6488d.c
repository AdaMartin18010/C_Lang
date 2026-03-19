/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md
 * Line: 352
 * Language: c
 * Block ID: b0a6488d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 通过时分复用减少操作符数量
 */

// 未优化：4个乘法器
void area_no_sharing(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        int p1 = a[i] * 2;
        int p2 = b[i] * 3;
        int p3 = a[i] * b[i];
        int p4 = c[i] * 4;
        c[i] = p1 + p2 + p3 + p4;
    }
}
// DSP: 4

// 优化1：通过放宽II实现共享
void area_sharing_ii(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=2
        // 2个乘法器时分复用
        int p1 = a[i] * 2;
        int p2 = b[i] * 3;
        int p3 = a[i] * b[i];
        int p4 = c[i] * 4;
        c[i] = p1 + p2 + p3 + p4;
    }
}
// DSP: 2，延迟增加2x

// 优化2：显式分配限制
void area_allocation(int a[100], int b[100], int c[100]) {
    #pragma HLS ALLOCATION instances=mul limit=2 operation

    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        // 强制只使用2个乘法器
        c[i] = a[i] * b[i] + a[i] * 2 + b[i] * 3;
    }
}

// 优化3：常量优化为移位
void area_constant_opt(int a[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        // 乘以2的幂次转换为移位
        int mul2 = a[i] << 1;   // 不是乘法
        int mul4 = a[i] << 2;   // 不是乘法
        int mul8 = a[i] << 3;   // 不是乘法
        c[i] = mul2 + mul4 + mul8;
    }
}
// DSP: 0
