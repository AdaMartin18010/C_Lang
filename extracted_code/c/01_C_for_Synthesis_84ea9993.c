/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\01_C_for_Synthesis.md
 * Line: 286
 * Language: c
 * Block ID: 84ea9993
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ✅ 支持的指针用法

// 1. 数组指针（最常用）
void array_pointer(int *a, int n) {
    for (int i = 0; i < n; i++) {
        a[i] = a[i] * 2;
    }
}

// 2. 数组的数组（二维数组）
void matrix_multiply(int A[N][M], int B[M][P], int C[N][P]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            int sum = 0;
            for (int k = 0; k < M; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

// 3. 引用参数（C++）
void reference_param(int &a, int &b, int &sum) {
    sum = a + b;  // 引用在硬件中映射为端口
}

// ❌ 不支持的指针用法

// 1. 指针运算返回不可预测地址
void bad_pointer_arith(int *a, int offset) {
    int *p = a + (offset * offset);  // 复杂计算可能不被支持
    *p = 100;
}

// 2. 指针别名（Ambiguous）
void pointer_aliasing(int *a, int *b) {
    // HLS工具无法确定a和b是否指向同一块内存
    *a = 10;
    *b = 20;  // 可能的数据依赖
}

// ✅ 解决方案：使用restrict关键字
void no_aliasing(int *restrict a, int *restrict b) {
    *a = 10;
    *b = 20;  // 编译器可以确定无别名
}
