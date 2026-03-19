/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\12_Compiler_Optimization\04_Auto_Vectorization.md
 * Line: 64
 * Language: c
 * Block ID: ccd15a9d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ✅ 可向量化的模式

// 1. 简单循环
void scale_array(float *a, float s, int n) {
    for (int i = 0; i < n; i++) {
        a[i] *= s;
    }
}

// 2. 归约操作
float sum_array(float *a, int n) {
    float sum = 0;
    for (int i = 0; i < n; i++) {
        sum += a[i];
    }
    return sum;
}

// 3. 数组赋值
void copy_array(float *dst, float *src, int n) {
    for (int i = 0; i < n; i++) {
        dst[i] = src[i];
    }
}

// 4. 条件赋值
void threshold_array(float *a, float t, int n) {
    for (int i = 0; i < n; i++) {
        a[i] = (a[i] > t) ? a[i] : t;
    }
}
