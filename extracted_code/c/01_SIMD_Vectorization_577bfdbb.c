/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 1972
 * Language: c
 * Block ID: 577bfdbb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 循环展开深度选择
 */

// 2x展开
#define UNROLL2(i, n, body) \
    for (; i <= (n) - 2; i += 2) { body(i); body(i+1); }

// 4x展开 (最常用)
#define UNROLL4(i, n, body) \
    for (; i <= (n) - 4; i += 4) { body(i); body(i+1); body(i+2); body(i+3); }

// 8x展开 (高寄存器压力场景)
#define UNROLL8(i, n, body) \
    for (; i <= (n) - 8; i += 8) { \
        body(i); body(i+1); body(i+2); body(i+3); \
        body(i+4); body(i+5); body(i+6); body(i+7); \
    }

// 使用示例: 点积计算
float dot_unroll8(const float *a, const float *b, int n) {
    float sum[8] = {0};
    int i = 0;

    // 8x展开主循环
    for (; i <= n - 8; i += 8) {
        sum[0] += a[i]   * b[i];
        sum[1] += a[i+1] * b[i+1];
        sum[2] += a[i+2] * b[i+2];
        sum[3] += a[i+3] * b[i+3];
        sum[4] += a[i+4] * b[i+4];
        sum[5] += a[i+5] * b[i+5];
        sum[6] += a[i+6] * b[i+6];
        sum[7] += a[i+7] * b[i+7];
    }

    // 合并部分和
    float total = sum[0] + sum[1] + sum[2] + sum[3] +
                  sum[4] + sum[5] + sum[6] + sum[7];

    // epilogue
    for (; i < n; i++) total += a[i] * b[i];
    return total;
}
