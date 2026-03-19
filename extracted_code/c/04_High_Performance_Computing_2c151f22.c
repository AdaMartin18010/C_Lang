/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\04_High_Performance_Computing.md
 * Line: 130
 * Language: c
 * Block ID: 2c151f22
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 提示编译器向量化

// ❌ 阻碍向量化的代码
void bad_loop(float *a, float *b, int n) {
    for (int i = 0; i < n; i++) {
        if (a[i] > 0) {  // 条件分支阻碍向量化
            b[i] = a[i] * 2;
        } else {
            b[i] = a[i];
        }
    }
}

// ✅ 使用条件移动
void better_loop(float *a, float *b, int n) {
    for (int i = 0; i < n; i++) {
        // 使用条件选择，无分支
        b[i] = (a[i] > 0) ? (a[i] * 2) : a[i];
    }
}

// ✅ 使用restrict关键字
void vectorized_loop(float * restrict a,
                     float * restrict b, int n) {
    #pragma GCC ivdep  // 忽略向量依赖
    for (int i = 0; i < n; i++) {
        b[i] = a[i] * 2.0f;
    }
}

// 编译选项
// gcc -O3 -march=native -ftree-vectorize -fopt-info-vec
