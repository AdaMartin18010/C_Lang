/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\02_Randomized_Algorithms.md
 * Line: 318
 * Language: c
 * Block ID: 591a89b0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 随机数质量测试
#include <math.h>

// 频率测试（单比特）
double frequency_test(const uint8_t *data, size_t len) {
    int ones = 0;
    for (size_t i = 0; i < len; i++) {
        for (int j = 0; j < 8; j++) {
            if (data[i] & (1 << j)) ones++;
        }
    }

    int n = len * 8;
    double s = (double)ones / n;
    double s_obs = fabs(s - 0.5) * sqrt(12.0 * n);

    // p-value（简化计算）
    return erfc(s_obs / sqrt(2.0));
}

// 游程测试
typedef struct {
    int runs;
    int ones;
    int zeros;
} RunsResult;

RunsResult runs_test(const uint8_t *data, size_t len) {
    RunsResult result = {0};
    int prev_bit = -1;

    for (size_t i = 0; i < len; i++) {
        for (int j = 7; j >= 0; j--) {
            int bit = (data[i] >> j) & 1;
            if (bit) result.ones++;
            else result.zeros++;

            if (prev_bit != -1 && bit != prev_bit) {
                result.runs++;
            }
            prev_bit = bit;
        }
    }
    result.runs++;  // 加上最后一个游程

    return result;
}

// 自相关测试
double autocorrelation_test(const uint8_t *data, size_t len, int shift) {
    int count = 0;
    int n = len * 8 - shift;

    for (int i = 0; i < n; i++) {
        int bit1 = (data[i / 8] >> (7 - i % 8)) & 1;
        int bit2 = (data[(i + shift) / 8] >> (7 - (i + shift) % 8)) & 1;
        if (bit1 == bit2) count++;
    }

    return (double)count / n;
}
