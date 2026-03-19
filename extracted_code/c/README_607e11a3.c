/*
 * Auto-generated from: 04_Industrial_Scenarios\06_Quantum_Computing\README.md
 * Line: 637
 * Language: c
 * Block ID: 607e11a3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file quantum_rng.c
 * @brief 量子随机数生成器实现
 *
 * 此实现使用量子力学原理生成真随机数
 * 实际应用需要量子硬件或量子随机数服务API
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

/* 模拟量子比特状态 */
typedef struct {
    double prob_0;  /* 测量得到0的概率 */
    double prob_1;  /* 测量得到1的概率 */
    double phase;   /* 相位 */
} SimulatedQubit;

/**
 * @brief 初始化量子随机数生成器
 *
 * 使用当前时间作为初始熵源
 * 实际QRNG不需要种子，因为量子过程本身就是随机的
 */
void qrng_init(void) {
    /* 在模拟中仍使用srand作为熵源 */
    srand((unsigned)time(NULL) ^ clock());
}

/**
 * @brief 模拟量子比特测量
 *
 * 模拟叠加态 (|0⟩ + |1⟩)/√2 的测量
 * 真实QRNG使用量子硬件完成此操作
 */
int qrng_measure_qubit(void) {
    /*
     * 模拟量子测量过程
     * 对于平衡叠加态 |+⟩，P(0) = P(1) = 0.5
     * 量子测量的随机性是本质的，非伪随机
     */

    /* 使用多个熵源增加随机性（模拟量子噪声） */
    unsigned long entropy = 0;
    entropy ^= (unsigned long)time(NULL);
    entropy ^= (unsigned long)clock();
    entropy ^= (unsigned long)&entropy;  /* ASLR熵 */

    /* 模拟量子测量的概率分布 */
    double random_val = (double)(rand() ^ entropy) / RAND_MAX;

    /* 50/50概率（模拟|+⟩态测量） */
    return (random_val < 0.5) ? 0 : 1;
}

/**
 * @brief 生成单个随机比特
 */
int qrng_random_bit(void) {
    return qrng_measure_qubit();
}

/**
 * @brief 生成8位随机字节
 */
uint8_t qrng_random_byte(void) {
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++) {
        byte = (byte << 1) | qrng_random_bit();
    }
    return byte;
}

/**
 * @brief 生成32位随机整数
 */
uint32_t qrng_random_uint32(void) {
    uint32_t value = 0;
    for (int i = 0; i < 32; i++) {
        value = (value << 1) | qrng_random_bit();
    }
    return value;
}

/**
 * @brief 生成64位随机整数
 */
uint64_t qrng_random_uint64(void) {
    return ((uint64_t)qrng_random_uint32() << 32) | qrng_random_uint32();
}

/**
 * @brief 生成指定范围内的随机整数 [min, max]
 */
int qrng_random_range(int min, int max) {
    if (min >= max) return min;

    uint32_t range = (uint32_t)(max - min + 1);
    uint32_t mask = 1;
    while (mask < range - 1) {
        mask = (mask << 1) | 1;
    }

    uint32_t result;
    do {
        result = qrng_random_uint32() & mask;
    } while (result >= range);

    return min + (int)result;
}

/**
 * @brief 生成[0, 1)区间的随机浮点数
 */
double qrng_random_double(void) {
    /* 使用53位精度 */
    uint64_t value = qrng_random_uint64() >> 11;
    return (double)value / (1ULL << 53);
}

/**
 * @brief 填充随机字节缓冲区
 */
void qrng_fill_buffer(uint8_t *buffer, size_t length) {
    for (size_t i = 0; i < length; i++) {
        buffer[i] = qrng_random_byte();
    }
}

/**
 * @brief 随机性测试 - 频数测试
 */
void qrng_frequency_test(int samples) {
    int zeros = 0, ones = 0;

    for (int i = 0; i < samples; i++) {
        if (qrng_random_bit() == 0) {
            zeros++;
        } else {
            ones++;
        }
    }

    printf("=== 频数测试结果 ===\n");
    printf("总样本数: %d\n", samples);
    printf("0的个数: %d (%.2f%%)\n", zeros, 100.0 * zeros / samples);
    printf("1的个数: %d (%.2f%%)\n", ones, 100.0 * ones / samples);
    printf("偏差: %.4f%%\n", 100.0 * fabs(zeros - ones) / samples);
    printf("理想值: 50.00%% 每种结果\n\n");
}

/**
 * @brief 运行游程测试
 */
void qrng_runs_test(int samples) {
    if (samples < 2) return;

    int *bits = malloc(samples * sizeof(int));
    for (int i = 0; i < samples; i++) {
        bits[i] = qrng_random_bit();
    }

    int runs = 1;  /* 游程数 */
    for (int i = 1; i < samples; i++) {
        if (bits[i] != bits[i-1]) {
            runs++;
        }
    }

    printf("=== 游程测试结果 ===\n");
    printf("总样本数: %d\n", samples);
    printf("游程数: %d\n", runs);
    printf("平均游程长度: %.2f\n", (double)samples / runs);
    printf("(随机序列应有较多游程)\n\n");

    free(bits);
}

/* 主函数 - 演示 */
int main(void) {
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║         量子随机数生成器 (QRNG) 演示                    ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n\n");

    /* 初始化 */
    qrng_init();

    /* 随机性测试 */
    printf("进行随机性统计测试...\n\n");
    qrng_frequency_test(10000);
    qrng_runs_test(10000);

    /* 生成随机数示例 */
    printf("=== 随机数生成示例 ===\n\n");

    printf("随机比特序列 (64位):\n");
    for (int i = 0; i < 64; i++) {
        printf("%d", qrng_random_bit());
        if ((i + 1) % 8 == 0) printf(" ");
    }
    printf("\n\n");

    printf("随机字节 (16字节):\n");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", qrng_random_byte());
    }
    printf("\n\n");

    printf("随机整数:\n");
    printf("  uint32: %u\n", qrng_random_uint32());
    printf("  uint64: %llu\n", (unsigned long long)qrng_random_uint64());
    printf("  range [1,100]: %d\n", qrng_random_range(1, 100));
    printf("  double [0,1): %.6f\n", qrng_random_double());

    return 0;
}
