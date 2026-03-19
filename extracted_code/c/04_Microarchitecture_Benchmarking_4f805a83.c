/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\04_Microarchitecture_Benchmarking.md
 * Line: 199
 * Language: c
 * Block ID: 4f805a83
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* cache_latency_test.c - Cache延迟测试 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>

/* RDTSC指令封装 */
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

/* CPUID序列化 */
static inline void cpuid(void) {
    __asm__ __volatile__ ("cpuid" ::: "eax", "ebx", "ecx", "edx");
}

/* 内存屏障 */
static inline void mfence(void) {
    __asm__ __volatile__ ("mfence" ::: "memory");
}

/* 链表节点 */
struct node {
    struct node *next;
    uint64_t padding[7];  /* 64字节对齐，填充到缓存行大小 */
};

/* 创建随机链表 */
struct node* create_random_list(size_t num_nodes) {
    struct node *nodes = aligned_alloc(64, num_nodes * sizeof(struct node));
    if (!nodes) {
        perror("aligned_alloc");
        return NULL;
    }

    /* 初始化next为NULL */
    for (size_t i = 0; i < num_nodes; i++) {
        nodes[i].next = NULL;
    }

    /* 创建索引数组并随机打乱 */
    size_t *indices = malloc(num_nodes * sizeof(size_t));
    for (size_t i = 0; i < num_nodes; i++) {
        indices[i] = i;
    }

    /* Fisher-Yates洗牌 */
    for (size_t i = num_nodes - 1; i > 0; i--) {
        size_t j = rand() % (i + 1);
        size_t temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    /* 创建随机链表 */
    for (size_t i = 0; i < num_nodes - 1; i++) {
        nodes[indices[i]].next = &nodes[indices[i + 1]];
    }
    nodes[indices[num_nodes - 1]].next = &nodes[indices[0]];  /* 循环 */

    free(indices);
    return nodes;
}

/* 测量链表遍历延迟 */
uint64_t measure_latency(struct node *head, size_t iterations) {
    struct node *p = head;
    uint32_t aux;

    /* 预热 */
    for (size_t i = 0; i < 1000; i++) {
        p = p->next;
    }

    /* 序列化 */
    cpuid();
    mfence();

    uint64_t start = rdtscp(&aux);

    /* 遍历链表 - 关键循环 */
    for (size_t i = 0; i < iterations; i++) {
        p = p->next;  /* 依赖链，无法并行 */
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;  /* 16次展开 */
    }

    uint64_t end = rdtscp(&aux);

    /* 防止优化 */
    volatile void *dummy = p;
    (void)dummy;

    return (end - start) / (iterations * 16);
}

/* 绑定到特定CPU */
void bind_to_cpu(int cpu) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);

    if (sched_setaffinity(0, sizeof(cpuset), &cpuset) != 0) {
        perror("sched_setaffinity");
    }
}

/* 获取CPU频率 */
double get_cpu_frequency(void) {
    /* 尝试从/proc读取 */
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "cpu MHz", 7) == 0) {
                double freq;
                sscanf(line, "cpu MHz : %lf", &freq);
                fclose(fp);
                return freq * 1e6;
            }
        }
        fclose(fp);
    }

    /* 默认假设3GHz */
    return 3.0e9;
}

/* 清空缓存 */
void flush_cache(void) {
    static size_t flush_size = 64 * 1024 * 1024;  /* 64MB */
    static char *flush_buffer = NULL;

    if (!flush_buffer) {
        flush_buffer = malloc(flush_size);
    }

    volatile char sum = 0;
    for (size_t i = 0; i < flush_size; i += 64) {
        sum += flush_buffer[i];
    }
    (void)sum;
}

/* 运行单次测试 */
void run_cache_test(const char *name, size_t size_kb, int is_cold) {
    size_t num_nodes = (size_kb * 1024) / sizeof(struct node);
    const size_t iterations = 1000000;

    struct node *list = create_random_list(num_nodes);
    if (!list) return;

    if (is_cold) {
        flush_cache();
    }

    uint64_t cycles = measure_latency(list, iterations);
    double freq = get_cpu_frequency();
    double nanoseconds = (cycles / freq) * 1e9;

    printf("%-20s %6zu KB  %s: %6.2f cycles (%6.2f ns)\n",
           name, size_kb, is_cold ? "Cold" : "Hot ",
           (double)cycles, nanoseconds);

    free(list);
}

int main(int argc, char *argv[]) {
    /* 设置随机种子 */
    srand(time(NULL));

    /* 绑定到CPU 0 */
    bind_to_cpu(0);

    printf("========================================\n");
    printf("      Cache延迟测试                     \n");
    printf("========================================\n\n");

    double freq_mhz = get_cpu_frequency() / 1e6;
    printf("CPU频率: %.0f MHz\n\n", freq_mhz);

    /* 热缓存测试 */
    printf("热缓存测试 (数据在Cache中):\n");
    printf("%-20s %10s  %6s: %10s (%10s)\n",
           "测试名称", "大小", "状态", "周期", "纳秒");
    printf("--------------------------------------------------------------------------------\n");

    run_cache_test("L1 Cache", 16, 0);
    run_cache_test("L1 Cache", 32, 0);
    run_cache_test("L2 Cache", 256, 0);
    run_cache_test("L2 Cache", 512, 0);
    run_cache_test("L3 Cache", 2048, 0);
    run_cache_test("L3 Cache", 4096, 0);
    run_cache_test("L3 Cache", 8192, 0);
    run_cache_test("Memory", 32768, 0);
    run_cache_test("Memory", 65536, 0);

    printf("\n");

    /* 冷缓存测试 */
    printf("冷缓存测试 (数据从内存加载):\n");
    printf("%-20s %10s  %6s: %10s (%10s)\n",
           "测试名称", "大小", "状态", "周期", "纳秒");
    printf("--------------------------------------------------------------------------------\n");

    run_cache_test("L1 Cache", 16, 1);
    run_cache_test("L1 Cache", 32, 1);
    run_cache_test("L2 Cache", 256, 1);
    run_cache_test("L2 Cache", 512, 1);
    run_cache_test("L3 Cache", 2048, 1);
    run_cache_test("L3 Cache", 4096, 1);
    run_cache_test("L3 Cache", 8192, 1);
    run_cache_test("Memory", 32768, 1);
    run_cache_test("Memory", 65536, 1);

    printf("\n========================================\n");
    printf("预期参考值:\n");
    printf("  L1 Cache:  3-5  周期 (约 1-2  ns)\n");
    printf("  L2 Cache:  10-15 周期 (约 3-5  ns)\n");
    printf("  L3 Cache:  30-50 周期 (约 10-15 ns)\n");
    printf("  Memory:    200-300 周期 (约 60-100 ns)\n");
    printf("========================================\n");

    return 0;
}
