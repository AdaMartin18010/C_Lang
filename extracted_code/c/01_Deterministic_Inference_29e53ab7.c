/*
 * Auto-generated from: 16_Embedded_AI_Agents\08_Real_Time_AI\01_Deterministic_Inference.md
 * Line: 132
 * Language: c
 * Block ID: 29e53ab7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 定点数类型定义
typedef int16_t q15_t;   // Q15格式: 1.15 (1符号位, 15小数位)
typedef int32_t q31_t;   // Q31格式: 1.31
typedef int8_t q7_t;     // Q7格式: 1.7

// Q15乘法 ( saturating multiplication )
static inline q15_t q15_mul(q15_t a, q15_t b) {
    // (a * b) >> 15，带饱和
    int32_t temp = (int32_t)a * (int32_t)b;
    temp = (temp + (1 << 14)) >> 15;  // 四舍五入

    // 饱和处理
    if (temp > INT16_MAX) return INT16_MAX;
    if (temp < INT16_MIN) return INT16_MIN;
    return (q15_t)temp;
}

// Q15加法 ( saturating addition )
static inline q15_t q15_add(q15_t a, q15_t b) {
    int32_t temp = (int32_t)a + (int32_t)b;
    if (temp > INT16_MAX) return INT16_MAX;
    if (temp < INT16_MIN) return INT16_MIN;
    return (q15_t)temp;
}

// 定点数矩阵乘法 (确定性执行)
void mat_mul_fixed(const q15_t* a, const q15_t* b, q15_t* c,
                   int m, int n, int k) {
    // m x k @ k x n = m x n
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            q31_t sum = 0;  // 使用32位累加避免溢出
            for (int l = 0; l < k; l++) {
                sum += (q31_t)a[i * k + l] * (q31_t)b[l * n + j];
            }
            // 转换回Q15并饱和
            sum = (sum + (1 << 14)) >> 15;
            if (sum > INT16_MAX) sum = INT16_MAX;
            if (sum < INT16_MIN) sum = INT16_MIN;
            c[i * n + j] = (q15_t)sum;
        }
    }
}

// ReLU激活 (定点数)
void relu_fixed(q15_t* data, int size) {
    for (int i = 0; i < size; i++) {
        data[i] = (data[i] > 0) ? data[i] : 0;
    }
}
