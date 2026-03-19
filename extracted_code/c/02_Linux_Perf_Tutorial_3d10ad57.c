/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\02_Linux_Perf_Tutorial.md
 * Line: 1053
 * Language: c
 * Block ID: 3d10ad57
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* flame_test.c - 用于生成火焰图的测试程序 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

/* 模拟不同层级的CPU消耗 */

void heavy_computation(int n) {
    volatile double result = 0;
    for (int i = 0; i < n * 1000; i++) {
        result += sin(i) * cos(i);
    }
    (void)result;
}

void medium_computation(int n) {
    volatile double result = 0;
    for (int i = 0; i < n * 500; i++) {
        result += log(i + 1) * sqrt(i + 1);
    }
    heavy_computation(n / 2);
    (void)result;
}

void light_computation(int n) {
    volatile int sum = 0;
    for (int i = 0; i < n * 100; i++) {
        sum += i;
    }
    medium_computation(n / 2);
    (void)sum;
}

/* 数据处理函数 */
void process_data_a(int *data, int n) {
    for (int i = 0; i < n; i++) {
        data[i] = data[i] * 2 + 1;
    }
    heavy_computation(100);
}

void process_data_b(int *data, int n) {
    for (int i = 0; i < n; i++) {
        data[i] = data[i] / 2 - 1;
    }
    medium_computation(100);
}

void process_data_c(int *data, int n) {
    for (int i = 0; i < n; i++) {
        data[i] = (data[i] << 2) ^ 0xFF;
    }
    light_computation(100);
}

/* 主处理流程 */
void data_processing_pipeline(int *data, int n, int iterations) {
    for (int iter = 0; iter < iterations; iter++) {
        process_data_a(data, n);
        process_data_b(data, n);
        process_data_c(data, n);
    }
}

/* 内存操作函数 */
void memory_heavy_operation(int size) {
    int *buffer = malloc(size * sizeof(int));
    if (!buffer) return;

    /* 写入操作 */
    for (int i = 0; i < size; i++) {
        buffer[i] = i;
    }

    /* 读取操作 */
    volatile int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += buffer[i];
    }

    free(buffer);
    (void)sum;
}

void memory_light_operation(int size) {
    int *buffer = malloc(size * sizeof(int) / 10);
    if (!buffer) return;

    for (int i = 0; i < size / 10; i++) {
        buffer[i] = i;
    }

    free(buffer);
}

/* I/O操作模拟 */
void io_operation_simulation(int n) {
    FILE *fp = tmpfile();
    if (!fp) return;

    char buffer[4096];
    memset(buffer, 'A', sizeof(buffer));

    for (int i = 0; i < n; i++) {
        fwrite(buffer, 1, sizeof(buffer), fp);
    }

    fflush(fp);
    rewind(fp);

    for (int i = 0; i < n; i++) {
        fread(buffer, 1, sizeof(buffer), fp);
    }

    fclose(fp);
}

/* 混合工作负载 */
void mixed_workload(int iterations) {
    const int DATA_SIZE = 100000;
    int *data = malloc(DATA_SIZE * sizeof(int));

    for (int i = 0; i < iterations; i++) {
        /* 初始化数据 */
        for (int j = 0; j < DATA_SIZE; j++) {
            data[j] = rand();
        }

        /* CPU密集处理 */
        data_processing_pipeline(data, DATA_SIZE, 10);

        /* 内存操作 */
        memory_heavy_operation(100000);
        memory_light_operation(100000);

        /* I/O操作 */
        if (i % 10 == 0) {
            io_operation_simulation(100);
        }
    }

    free(data);
}

int main(int argc, char *argv[]) {
    printf("火焰图测试程序启动...\n");
    printf("请使用: perf record -F 99 -g -- ./flame_test\n");
    printf("然后: perf script | stackcollapse-perf.pl | flamegraph.pl > flame_test.svg\n\n");

    int iterations = 100;
    if (argc > 1) {
        iterations = atoi(argv[1]);
    }

    printf("执行 %d 次迭代...\n", iterations);

    mixed_workload(iterations);

    printf("完成!\n");
    return 0;
}
