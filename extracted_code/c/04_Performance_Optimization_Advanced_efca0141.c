/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 316
 * Language: c
 * Block ID: efca0141
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file branch_predictor_analysis.c
 * @brief 现代CPU分支预测器行为分析
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

/* 分支预测器类型模拟 */
typedef enum {
    PREDICTOR_STATIC,       /* 静态预测：总是预测不跳转 */
    PREDICTOR_BIMODAL,      /* 双模态预测器：基于历史 */
    PREDICTOR_GSHARE,       /* Gshare：全局历史异或PC */
    PREDICTOR_PERCEPTRON    /* 感知机预测器：神经网络 */
} PredictorType;

/**
 * @brief 分支模式分析
 *
 * 不同模式对分支预测器的影响：
 * - T-T-T-T: 100%预测成功（学习后）
 * - N-N-N-N: 100%预测成功（学习后）
 * - T-N-T-N: 可能振荡，取决于历史位数
 * - 随机: ~50%成功率
 */
void analyze_branch_patterns(void) {
    const int N = 100000000;
    int *patterns = malloc(N * sizeof(int));

    printf("=== Branch Pattern Analysis ===\n\n");

    /* 模式1: 总是跳转 */
    printf("Pattern 1: Always Taken\n");
    volatile long long sum = 0;
    clock_t start = clock();
    for (int i = 0; i < N; i++) {
        if (1) {  /* 编译器可能优化掉 */
            sum += i;
        }
    }
    printf("  Cycles: %ld\n\n", clock() - start);

    /* 模式2: 交替跳转 (T-N-T-N) */
    printf("Pattern 2: Alternating (T-N-T-N)\n");
    sum = 0;
    start = clock();
    for (int i = 0; i < N; i++) {
        if (i & 1) {  /* 依赖最低位 */
            sum += i;
        }
    }
    printf("  Cycles: %ld\n\n", clock() - start);

    /* 模式3: 每4次跳转一次 */
    printf("Pattern 3: Every 4th iteration\n");
    sum = 0;
    start = clock();
    for (int i = 0; i < N; i++) {
        if ((i & 3) == 0) {
            sum += i;
        }
    }
    printf("  Cycles: %ld\n\n", clock() - start);

    /* 模式4: 随机 */
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        patterns[i] = rand() & 1;
    }

    printf("Pattern 4: Random\n");
    sum = 0;
    start = clock();
    for (int i = 0; i < N; i++) {
        if (patterns[i]) {
            sum += i;
        }
    }
    printf("  Cycles: %ld\n\n", clock() - start);

    free(patterns);
}

/**
 * @brief 间接跳转预测
 *
 * 间接跳转（如函数指针、虚函数表）更难预测
 * 现代CPU使用目标地址缓存（BTB）
 */
typedef void (*func_ptr_t)(int);

void func_a(int x) { volatile int y = x * 2; (void)y; }
void func_b(int x) { volatile int y = x * 3; (void)y; }
void func_c(int x) { volatile int y = x * 5; (void)y; }
void func_d(int x) { volatile int y = x * 7; (void)y; }

void indirect_branch_analysis(void) {
    const int N = 50000000;
    func_ptr_t funcs[] = {func_a, func_b, func_c, func_d};

    printf("=== Indirect Branch Analysis ===\n\n");

    /* 有序调用 - 模式可学习 */
    printf("Ordered indirect calls:\n");
    clock_t start = clock();
    for (int i = 0; i < N; i++) {
        funcs[i % 4](i);  /* A-B-C-D-A-B-C-D模式 */
    }
    printf("  Cycles: %ld\n\n", clock() - start);

    /* 随机调用 - 难以预测 */
    srand(42);
    int *indices = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) {
        indices[i] = rand() % 4;
    }

    printf("Random indirect calls:\n");
    start = clock();
    for (int i = 0; i < N; i++) {
        funcs[indices[i]](i);
    }
    printf("  Cycles: %ld\n", clock() - start);

    free(indices);
}

int main(void) {
    analyze_branch_patterns();
    indirect_branch_analysis();
    return 0;
}
