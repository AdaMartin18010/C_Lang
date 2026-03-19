/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\03_Top_Down_Analysis_Method.md
 * Line: 666
 * Language: c
 * Block ID: b7d5de27
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* core_bound_test.c - Core Bound测试程序 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* 测试1: 除法运算密集型 (Divider瓶颈) */
void divider_heavy(double *data, int n) {
    volatile double result = 1.0;
    for (int i = 0; i < n; i++) {
        /* 整数除法非常慢 */
        result = result / (1.0 + data[i] * 0.0001);
    }
}

/* 测试2: 除法优化版本 (使用乘法代替) */
void divider_optimized(double *data, int n) {
    volatile double result = 1.0;
    for (int i = 0; i < n; i++) {
        /* 预计算倒数，使用乘法 */
        double inv = 1.0 / (1.0 + data[i] * 0.0001);
        result = result * inv;
    }
}

/* 测试3: 端口压力测试 - 大量独立整数运算 */
void port_pressure_test(int *data, int n) {
    volatile int r1 = 0, r2 = 0, r3 = 0, r4 = 0;
    for (int i = 0; i < n; i++) {
        /* 多个独立运算，可同时使用多个端口 */
        r1 += data[i] * 2;
        r2 -= data[i] * 3;
        r3 += data[i] << 2;
        r4 ^= data[i] >> 1;
    }
    (void)r1; (void)r2; (void)r3; (void)r4;
}

/* 测试4: 数据依赖性链 (Core Bound) */
void dependency_chain(double *data, int n) {
    volatile double result = 0;
    for (int i = 0; i < n; i++) {
        /* 长依赖链，每个操作依赖前一个结果 */
        result = result + data[i];
        result = result * 1.0001;
        result = result - data[i] * 0.5;
        result = result * 0.9999;
    }
}

/* 测试5: 打破依赖链 */
void dependency_broken(double *data, int n) {
    volatile double r1 = 0, r2 = 0, r3 = 0, r4 = 0;
    for (int i = 0; i < n; i += 4) {
        /* 4个独立依赖链，可同时执行 */
        r1 = r1 + data[i];
        r2 = r2 + data[i+1];
        r3 = r3 + data[i+2];
        r4 = r4 + data[i+3];
    }
    volatile double result = r1 + r2 + r3 + r4;
    (void)result;
}

/* 测试6: 浮点运算压力 */
void fp_pressure_test(double *data, int n) {
    volatile double r1 = 0, r2 = 0, r3 = 0;
    for (int i = 0; i < n; i++) {
        /* 密集浮点运算 */
        r1 = r1 * 1.1 + data[i];
        r2 = r2 * 0.9 - data[i] * 0.5;
        r3 = r3 + r1 * r2;
    }
    (void)r1; (void)r2; (void)r3;
}

/* 测试7: 向量运算优化 (SIMD) */
void vector_optimized(float *data, int n) {
    /* 假设使用AVX，一次处理8个float */
    volatile float sum[8] = {0};
    for (int i = 0; i < n; i += 8) {
        for (int j = 0; j < 8; j++) {
            sum[j] += data[i + j];
        }
    }
    volatile float total = sum[0] + sum[1] + sum[2] + sum[3] +
                          sum[4] + sum[5] + sum[6] + sum[7];
    (void)total;
}

int main() {
    const int N = 10000000;
    double *double_data = aligned_alloc(64, N * sizeof(double));
    float *float_data = aligned_alloc(64, N * sizeof(float));
    int *int_data = aligned_alloc(64, N * sizeof(int));

    for (int i = 0; i < N; i++) {
        double_data[i] = (double)rand() / RAND_MAX;
        float_data[i] = (float)rand() / RAND_MAX;
        int_data[i] = rand();
    }

    printf("Core Bound测试开始...\n");
    printf("建议监控事件:\n");
    printf("  - CYCLE_ACTIVITY.STALLS_TOTAL\n");
    printf("  - UOPS_EXECUTED.CORE\n");
    printf("  - UOPS_DISPATCHED.PORT_0/1/2/3/4/5/6/7\n");
    printf("  - ARITH.DIVIDER_ACTIVE\n\n");

    divider_heavy(double_data, N);
    divider_optimized(double_data, N);
    port_pressure_test(int_data, N);
    dependency_chain(double_data, N);
    dependency_broken(double_data, N);
    fp_pressure_test(double_data, N);
    vector_optimized(float_data, N);

    printf("测试完成!\n");

    free(double_data);
    free(float_data);
    free(int_data);

    return 0;
}
