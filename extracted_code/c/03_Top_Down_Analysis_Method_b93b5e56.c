/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\03_Top_Down_Analysis_Method.md
 * Line: 852
 * Language: c
 * Block ID: b93b5e56
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* bad_speculation_test.c - Bad Speculation测试程序 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* 测试1: 可预测分支 (高效) */
void predictable_branch(int *data, int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        /* 总是为真，完全可预测 */
        if (i >= 0) {
            sum += data[i];
        }
    }
}

/* 测试2: 随机分支 (低效) */
void random_branch(int *data, int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        /* 随机模式，难以预测 */
        if (rand() % 2) {
            sum += data[i];
        }
    }
}

/* 测试3: 交替分支 (最难预测) */
void alternating_branch(int *data, int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        /* 严格交替: T, F, T, F... */
        if (i & 1) {
            sum += data[i];
        }
    }
}

/* 测试4: 数据相关分支 - 未排序 */
void unsorted_data_branch(int *data, int n) {
    volatile int count = 0;
    for (int i = 0; i < n; i++) {
        /* 分支方向取决于数据值 */
        if (data[i] > 100) {
            count++;
        }
    }
}

/* 测试5: 数据相关分支 - 已排序 (优化) */
void sorted_data_branch(int *data, int n) {
    volatile int count = 0;
    /* 先排序数据 - 使分支更可预测 */
    qsort(data, n, sizeof(int),
          (int (*)(const void*, const void*))strcmp);

    for (int i = 0; i < n; i++) {
        if (data[i] > 100) {
            count++;
        }
    }
}

/* 测试6: 间接跳转 - 随机 */
void indirect_jump_random(int n) {
    void (*funcs[])(void) = {
        (void(*)(void))puts,
        (void(*)(void))exit,
        (void(*)(void))malloc,
        (void(*)(void))free
    };

    for (int i = 0; i < n; i++) {
        /* 随机选择函数，难以预测 */
        funcs[rand() % 4]();
    }
}

/* 测试7: 间接跳转 - 模式化 (更好预测) */
void indirect_jump_pattern(int n) {
    void (*funcs[])(void) = {
        (void(*)(void))puts,
        (void(*)(void))exit,
        (void(*)(void))malloc,
        (void(*)(void))free
    };

    for (int i = 0; i < n; i++) {
        /* 循环模式，易于预测 */
        funcs[i % 4]();
    }
}

/* 测试8: 多分支switch - 随机 */
int switch_random(int n) {
    volatile int result = 0;
    for (int i = 0; i < n; i++) {
        int r = rand() % 8;
        switch (r) {
            case 0: result += 1; break;
            case 1: result += 2; break;
            case 2: result += 3; break;
            case 3: result += 4; break;
            case 4: result += 5; break;
            case 5: result += 6; break;
            case 6: result += 7; break;
            case 7: result += 8; break;
        }
    }
    return result;
}

/* 测试9: 内联vs函数调用 */
static inline int inline_add(int a, int b) {
    return a + b;
}

int normal_add(int a, int b) {
    return a + b;
}

void test_inline(int n) {
    volatile int result = 0;
    for (int i = 0; i < n; i++) {
        result = inline_add(result, i);
    }
}

void test_normal_call(int n) {
    volatile int result = 0;
    for (int i = 0; i < n; i++) {
        result = normal_add(result, i);
    }
}

int main() {
    const int N = 100000000;
    int *data = malloc(N * sizeof(int));

    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        data[i] = rand() % 200;
    }

    printf("Bad Speculation测试开始...\n");
    printf("建议监控事件:\n");
    printf("  - BR_MISP_RETIRED.ALL_BRANCHES\n");
    printf("  - BR_MISP_RETIRED.COND\n");
    printf("  - BR_MISP_RETIRED.INDIRECT\n");
    printf("  - MACHINE_CLEARS.ALL\n");
    printf("  - INT_MISC.RECOVERY_CYCLES\n\n");

    predictable_branch(data, N);
    random_branch(data, N);
    alternating_branch(data, N);
    unsorted_data_branch(data, N);

    /* 复制数据用于排序测试 */
    int *data_copy = malloc(N * sizeof(int));
    memcpy(data_copy, data, N * sizeof(int));
    sorted_data_branch(data_copy, N);
    free(data_copy);

    test_inline(N);
    test_normal_call(N);

    printf("测试完成!\n");

    free(data);
    return 0;
}
