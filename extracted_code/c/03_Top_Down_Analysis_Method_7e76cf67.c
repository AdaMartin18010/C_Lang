/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\03_Top_Down_Analysis_Method.md
 * Line: 359
 * Language: c
 * Block ID: 7e76cf67
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* frontend_bound_test.c - Frontend Bound测试程序 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 测试1: ICache友好代码 (顺序执行) */
void icache_friendly(int *data, int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += data[i];
    }
}

/* 测试2: ICache不友好代码 (大量条件分支) */
void icache_unfriendly(int *data, int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        /* 大量分散的分支降低ICache效率 */
        if (i % 2 == 0) sum += data[i];
        else if (i % 3 == 0) sum -= data[i];
        else if (i % 5 == 0) sum += data[i] * 2;
        else if (i % 7 == 0) sum -= data[i] * 2;
        else sum += data[i] / 2;
    }
}

/* 测试3: 微码密集型操作 (字符串处理) */
void microcode_heavy(char *dest, const char *src, int n) {
    for (int i = 0; i < n; i++) {
        /* strcpy等字符串操作使用微码序列 */
        dest[i] = src[i];
        if (src[i] == '\0') break;
    }
}

/* 测试4: 宏融合友好的代码 */
void macro_fusion_friendly(int *data, int n) {
    volatile int count = 0;
    for (int i = 0; i < n; i++) {
        /* 比较+分支可宏融合为单个微操作 */
        if (data[i] > 100) {
            count++;
        }
    }
}

/* 测试5: 宏融合不友好的代码 */
void macro_fusion_unfriendly(int *data, int n) {
    volatile int count = 0;
    for (int i = 0; i < n; i++) {
        /* 复杂条件难以宏融合 */
        if (data[i] > 100 && data[i] < 200 && data[i] % 2 == 0) {
            count++;
        }
    }
}

/* 测试6: 复杂指令测试 */
void complex_instructions(double *data, int n) {
    volatile double result = 0;
    for (int i = 0; i < n; i++) {
        /* 除法和开方需要微码序列化 */
        result += data[i] / 3.14159;
        if (i % 100 == 0) {
            result = __builtin_sqrt(result);
        }
    }
}

int main() {
    const int N = 10000000;
    int *int_data = aligned_alloc(64, N * sizeof(int));
    double *double_data = aligned_alloc(64, N * sizeof(double));
    char *str_src = malloc(1000);
    char *str_dst = malloc(1000);

    /* 初始化 */
    for (int i = 0; i < N; i++) {
        int_data[i] = rand() % 300;
        double_data[i] = (double)rand() / RAND_MAX;
    }
    memset(str_src, 'A', 999);
    str_src[999] = '\0';

    printf("Frontend Bound测试开始...\n");
    printf("请使用: perf stat -e cycles,instructions,IDQ.MS_UOPS,IDQ.MITE_UOPS \\\n");
    printf("         -e ICACHE_16B.IFDATA_STALL,BACLEARS.ANY \\\n");
    printf("         ./frontend_bound_test\n\n");

    /* 运行测试 */
    icache_friendly(int_data, N);
    icache_unfriendly(int_data, N);
    microcode_heavy(str_dst, str_src, 1000);
    macro_fusion_friendly(int_data, N);
    macro_fusion_unfriendly(int_data, N);
    complex_instructions(double_data, N);

    printf("测试完成!\n");

    free(int_data);
    free(double_data);
    free(str_src);
    free(str_dst);

    return 0;
}
