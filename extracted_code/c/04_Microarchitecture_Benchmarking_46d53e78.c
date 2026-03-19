/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\04_Microarchitecture_Benchmarking.md
 * Line: 1792
 * Language: c
 * Block ID: 46d53e78
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* comprehensive_microbench.c - 综合微架构基准测试 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sched.h>

/* RDTSC封装 */
static inline uint64_t rdtsc(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

static inline uint64_t rdtscp(uint32_t *aux) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtscp" : "=a" (lo), "=d" (hi), "=c" (*aux));
    return ((uint64_t)hi << 32) | lo;
}

static inline void cpuid(void) {
    __asm__ __volatile__ ("cpuid" ::: "eax", "ebx", "ecx", "edx");
}

static inline void mfence(void) {
    __asm__ __volatile__ ("mfence" ::: "memory");
}

/* 绑定CPU */
void bind_cpu(int cpu) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);
    sched_setaffinity(0, sizeof(cpuset), &cpuset);
}

/* ==================== 测试函数 ==================== */

/* 1. CPU基础性能 */
void test_cpu_baseline(void) {
    printf("\n【1. CPU基础性能测试】\n");
    printf("--------------------------------------------------\n");

    const int N = 100000000;
    volatile long sum = 0;
    uint32_t aux;

    /* 整数加法 */
    cpuid(); mfence();
    uint64_t start = rdtscp(&aux);
    for (int i = 0; i < N; i++) {
        sum += i;
    }
    uint64_t end = rdtscp(&aux);
    (void)sum;

    double cycles_per_add = (double)(end - start) / N;
    printf("整数加法: %.3f 周期/操作\n", cycles_per_add);

    /* 整数乘法 */
    sum = 1;
    cpuid(); mfence();
    start = rdtscp(&aux);
    for (int i = 0; i < N; i++) {
        sum *= (i % 100 + 1);
    }
    end = rdtscp(&aux);
    (void)sum;

    double cycles_per_mul = (double)(end - start) / N;
    printf("整数乘法: %.3f 周期/操作\n", cycles_per_mul);

    /* 整数除法 */
    sum = 1000000000;
    cpuid(); mfence();
    start = rdtscp(&aux);
    for (int i = 0; i < N / 100; i++) {
        sum /= (i % 100 + 1);
        sum += 1000000000;
    }
    end = rdtscp(&aux);
    (void)sum;

    double cycles_per_div = (double)(end - start) / (N / 100);
    printf("整数除法: %.3f 周期/操作\n", cycles_per_div);

    /* 浮点运算 */
    volatile double fsum = 0.0;
    cpuid(); mfence();
    start = rdtscp(&aux);
    for (int i = 0; i < N; i++) {
        fsum += (double)i * 0.001;
    }
    end = rdtscp(&aux);
    (void)fsum;

    double cycles_per_fadd = (double)(end - start) / N;
    printf("浮点加法: %.3f 周期/操作\n", cycles_per_fadd);

    /* 平方根 */
    fsum = 1.0;
    cpuid(); mfence();
    start = rdtscp(&aux);
    for (int i = 0; i < N / 10; i++) {
        fsum = sqrt(fsum + 1.0);
    }
    end = rdtscp(&aux);
    (void)fsum;

    double cycles_per_sqrt = (double)(end - start) / (N / 10);
    printf("平方根: %.3f 周期/操作\n", cycles_per_sqrt);
}

/* 2. 内存延迟测试 */
void test_memory_latency(void) {
    printf("\n【2. 内存延迟测试】\n");
    printf("--------------------------------------------------\n");

    struct node {
        struct node *next;
        uint64_t pad[7];
    };

    int sizes_kb[] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};
    int num_tests = sizeof(sizes_kb) / sizeof(sizes_kb[0]);

    for (int t = 0; t < num_tests; t++) {
        size_t num_nodes = (sizes_kb[t] * 1024) / sizeof(struct node);
        struct node *nodes = aligned_alloc(64, num_nodes * sizeof(struct node));

        /* 创建随机链表 */
        size_t *indices = malloc(num_nodes * sizeof(size_t));
        for (size_t i = 0; i < num_nodes; i++) indices[i] = i;
        for (size_t i = num_nodes - 1; i > 0; i--) {
            size_t j = rand() % (i + 1);
            size_t tmp = indices[i];
            indices[i] = indices[j];
            indices[j] = tmp;
        }
        for (size_t i = 0; i < num_nodes - 1; i++) {
            nodes[indices[i]].next = &nodes[indices[i + 1]];
        }
        nodes[indices[num_nodes - 1]].next = &nodes[indices[0]];
        free(indices);

        /* 测量 */
        struct node *p = nodes;
        const int iterations = 1000000;
        uint32_t aux;

        /* 预热 */
        for (int i = 0; i < 1000; i++) p = p->next;

        cpuid(); mfence();
        uint64_t start = rdtscp(&aux);
        for (int i = 0; i < iterations; i++) {
            p = p->next; p = p->next; p = p->next; p = p->next;
        }
        uint64_t end = rdtscp(&aux);
        (void)p;

        double latency = (double)(end - start) / (iterations * 4);

        char level[32];
        if (sizes_kb[t] <= 32) strcpy(level, "L1");
        else if (sizes_kb[t] <= 512) strcpy(level, "L2");
        else if (sizes_kb[t] <= 8192) strcpy(level, "L3");
        else strcpy(level, "Memory");

        printf("%5d KB [%s]: %.2f 周期\n", sizes_kb[t], level, latency);

        free(nodes);
    }
}

/* 3. 内存带宽测试 */
void test_memory_bandwidth(void) {
    printf("\n【3. 内存带宽测试】\n");
    printf("--------------------------------------------------\n");

    size_t size = 100 * 1024 * 1024;  /* 100MB */
    char *src = aligned_alloc(64, size);
    char *dst = aligned_alloc(64, size);
    memset(src, 0xAA, size);

    /* 顺序读取 */
    volatile uint64_t sum = 0;
    uint32_t aux;
    cpuid(); mfence();
    uint64_t start = rdtscp(&aux);
    for (size_t i = 0; i < size; i += 64) {
        sum += *(uint64_t*)(src + i);
    }
    uint64_t end = rdtscp(&aux);
    (void)sum;

    double read_bw = (double)size / ((end - start) / 3.0e9) / (1024 * 1024 * 1024);
    printf("顺序读取带宽: %.2f GB/s\n", read_bw);

    /* 顺序写入 */
    cpuid(); mfence();
    start = rdtscp(&aux);
    for (size_t i = 0; i < size; i += 64) {
        *(uint64_t*)(dst + i) = i;
    }
    end = rdtscp(&aux);

    double write_bw = (double)size / ((end - start) / 3.0e9) / (1024 * 1024 * 1024);
    printf("顺序写入带宽: %.2f GB/s\n", write_bw);

    /* 复制带宽 */
    cpuid(); mfence();
    start = rdtscp(&aux);
    memcpy(dst, src, size);
    end = rdtscp(&aux);

    double copy_bw = (double)(size * 2) / ((end - start) / 3.0e9) / (1024 * 1024 * 1024);
    printf("复制带宽: %.2f GB/s\n", copy_bw);

    free(src);
    free(dst);
}

/* 4. 分支预测测试 */
void test_branch_prediction(void) {
    printf("\n【4. 分支预测测试】\n");
    printf("--------------------------------------------------\n");

    const int N = 100000000;
    int *sorted = malloc(N * sizeof(int));
    int *random = malloc(N * sizeof(int));

    for (int i = 0; i < N; i++) {
        sorted[i] = i % 200;
        random[i] = rand() % 200;
    }

    volatile int count = 0;
    uint32_t aux;
    uint64_t start, end;

    /* 排序数据 */
    cpuid(); mfence();
    start = rdtscp(&aux);
    for (int i = 0; i < N; i++) {
        if (sorted[i] > 100) count++;
    }
    end = rdtscp(&aux);

    double sorted_time = (double)(end - start) / N;
    printf("排序数据分支: %.3f 周期/迭代\n", sorted_time);

    /* 随机数据 */
    count = 0;
    cpuid(); mfence();
    start = rdtscp(&aux);
    for (int i = 0; i < N; i++) {
        if (random[i] > 100) count++;
    }
    end = rdtscp(&aux);
    (void)count;

    double random_time = (double)(end - start) / N;
    printf("随机数据分支: %.3f 周期/迭代\n", random_time);
    printf("预测失败惩罚: %.1f 周期\n", random_time - sorted_time);

    free(sorted);
    free(random);
}

/* 5. 预取器测试 */
void test_prefetcher(void) {
    printf("\n【5. 预取器效率测试】\n");
    printf("--------------------------------------------------\n");

    size_t size = 100 * 1024 * 1024;
    int *arr = aligned_alloc(64, size);

    for (size_t i = 0; i < size / sizeof(int); i++) {
        arr[i] = i;
    }

    uint32_t aux;
    uint64_t start, end;

    /* 顺序访问 */
    volatile int sum = 0;
    cpuid(); mfence();
    start = rdtscp(&aux);
    for (size_t i = 0; i < size / sizeof(int); i++) {
        sum += arr[i];
    }
    end = rdtscp(&aux);

    double seq_cycles_per_access = (double)(end - start) / (size / sizeof(int));
    printf("顺序访问: %.3f 周期/访问\n", seq_cycles_per_access);

    /* 步幅64访问 (缓存行步幅) */
    sum = 0;
    cpuid(); mfence();
    start = rdtscp(&aux);
    for (size_t i = 0; i < size / sizeof(int); i += 16) {  /* 64字节步幅 */
        sum += arr[i];
    }
    end = rdtscp(&aux);
    (void)sum;

    double stride64_cycles = (double)(end - start) / (size / sizeof(int) / 16);
    printf("步幅64访问: %.3f 周期/访问\n", stride64_cycles);

    /* 步幅4096访问 (页面步幅，预取器失效) */
    sum = 0;
    cpuid(); mfence();
    start = rdtscp(&aux);
    for (size_t i = 0; i < size / sizeof(int); i += 1024) {  /* 4096字节步幅 */
        sum += arr[i];
    }
    end = rdtscp(&aux);

    double stride4k_cycles = (double)(end - start) / (size / sizeof(int) / 1024);
    printf("步幅4K访问: %.3f 周期/访问\n", stride4k_cycles);

    printf("预取器加速比: %.1fx\n", stride4k_cycles / seq_cycles_per_access);

    free(arr);
}

/* 主函数 */
int main(int argc, char *argv[]) {
    printf("========================================\n");
    printf("    综合微架构基准测试套件               \n");
    printf("========================================\n");

    /* 绑定CPU并设置优先级 */
    bind_cpu(0);

    /* 禁用输出缓冲 */
    setbuf(stdout, NULL);

    /* 运行所有测试 */
    test_cpu_baseline();
    test_memory_latency();
    test_memory_bandwidth();
    test_branch_prediction();
    test_prefetcher();

    printf("\n========================================\n");
    printf("测试完成!\n");
    printf("========================================\n");

    return 0;
}
