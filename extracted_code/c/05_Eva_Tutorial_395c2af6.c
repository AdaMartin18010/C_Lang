/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Eva_Tutorial.md
 * Line: 578
 * Language: c
 * Block ID: 395c2af6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// tutorial6_float.c
#include <math.h>

// 浮点精度问题示例
float unstable_sum(float arr[], int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        sum += arr[i];  // 大数吃小数问题
    }
    return sum;
}

// Kahan求和算法 (提高精度)
float kahan_sum(float arr[], int n) {
    float sum = 0.0f;
    float c = 0.0f;  // 补偿变量

    for (int i = 0; i < n; i++) {
        float y = arr[i] - c;
        float t = sum + y;
        c = (t - sum) - y;
        sum = t;
    }
    return sum;
}
