/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 455
 * Language: c
 * Block ID: 8b24af11
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file execution_ports.c
 * @brief CPU执行端口分析与优化
 *
 * Intel Skylake微架构端口分配：
 * - Port 0: ALU, Vec ALU, Vec Mul, Branch
 * - Port 1: ALU, Vec ALU, Vec Mul, Vec Shuffle
 * - Port 2: Load
 * - Port 3: Load
 * - Port 4: Store
 * - Port 5: ALU, Vec ALU, Vec Shuffle
 * - Port 6: ALU, Branch
 * - Port 7: Store AGU
 */

#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

/* 确保编译器不优化 */
#define NO_INLINE __attribute__((noinline))
#define OPTIMIZE_O3 __attribute__((optimize("-O3")))

/**
 * @brief 端口压力测试 - 整数ALU
 *
 * Skylake有4个ALU端口(0,1,5,6)
 * 最多每周期执行4个独立整数操作
 */
NO_INLINE uint64_t alu_port_pressure_test(int n) {
    uint64_t a = 1, b = 2, c = 3, d = 4;
    uint64_t start = __rdtsc();

    for (int i = 0; i < n; i++) {
        /* 4个独立加法，可并行执行 */
        a = a + 1;
        b = b + 2;
        c = c + 3;
        d = d + 4;
    }

    uint64_t end = __rdtsc();
    printf("4-way ALU: %lu cycles/iter\n", (end - start) / n);
    return a + b + c + d;
}

/**
 * @brief 端口压力测试 - 超过4个ALU操作
 */
NO_INLINE uint64_t alu_port_overload_test(int n) {
    uint64_t a = 1, b = 2, c = 3, d = 4, e = 5;
    uint64_t start = __rdtsc();

    for (int i = 0; i < n; i++) {
        /* 5个加法需要至少2个周期 */
        a = a + 1;
        b = b + 2;
        c = c + 3;
        d = d + 4;
        e = e + 5;
    }

    uint64_t end = __rdtsc();
    printf("5-way ALU: %lu cycles/iter\n", (end - start) / n);
    return a + b + c + d + e;
}

/**
 * @brief Load/Store端口测试
 *
 * 2个Load端口 + 1个Store端口
 * 理论上每周期可执行2 loads + 1 store
 */
NO_INLINE uint64_t memory_port_test(int n) {
    volatile uint64_t data[8] = {1,2,3,4,5,6,7,8};
    uint64_t sum = 0;
    uint64_t start = __rdtsc();

    for (int i = 0; i < n; i++) {
        /* 2 loads + 1 store */
        uint64_t t1 = data[0];
        uint64_t t2 = data[1];
        data[2] = t1 + t2;

        /* 指针算术保持循环依赖 */
        sum += data[2];
    }

    uint64_t end = __rdtsc();
    printf("Load/Store mix: %lu cycles/iter\n", (end - start) / n);
    return sum;
}

/**
 * @brief 乘除法端口压力
 *
 * 乘法使用端口0和1
 * 延迟3-4周期，吞吐量0.5（每周期2个）
 */
NO_INLINE uint64_t multiplication_throughput_test(int n) {
    uint64_t a = 1, b = 2, c = 3, d = 4;
    uint64_t start = __rdtsc();

    for (int i = 0; i < n; i++) {
        /* 4个独立乘法，2个端口 */
        a = a * 3;
        b = b * 5;
        c = c * 7;
        d = d * 11;
    }

    uint64_t end = __rdtsc();
    printf("4-way multiply: %lu cycles/iter (expected ~2)\n", (end - start) / n);
    return a + b + c + d;
}

int main(void) {
    printf("=== CPU Execution Port Analysis ===\n\n");

    printf("--- ALU Port Pressure ---\n");
    volatile uint64_t r1 = alu_port_pressure_test(100000000);
    volatile uint64_t r2 = alu_port_overload_test(100000000);
    (void)r1; (void)r2;

    printf("\n--- Memory Port Pressure ---\n");
    volatile uint64_t r3 = memory_port_test(100000000);
    (void)r3;

    printf("\n--- Multiplication Throughput ---\n");
    volatile uint64_t r4 = multiplication_throughput_test(100000000);
    (void)r4;

    return 0;
}
