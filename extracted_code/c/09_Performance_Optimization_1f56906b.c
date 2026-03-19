/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\09_Performance_Optimization.md
 * Line: 633
 * Language: c
 * Block ID: 1f56906b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 可以向量化的循环
void good_vectorization(float *a, float *b, float *c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];  // 简单, 无依赖
    }
}

// 阻碍向量化的因素
void bad_vectorization(float *a, int n) {
    for (int i = 0; i < n; i++) {
        a[i] = a[i-1] + 1;   // 循环依赖!
    }

    for (int i = 0; i < n; i++) {
        if (a[i] > 0)        // 条件分支
            a[i] = sqrt(a[i]);
    }

    float sum = 0;
    for (int i = 0; i < n; i++) {
        sum += a[i];         // 归约, 需要特殊处理
    }
}

// 帮助编译器
#pragma GCC ivdep  // 忽略向量依赖 (确保安全!)
for (int i = 0; i < n; i++) {
    a[i] = b[i] * c[i];
}
