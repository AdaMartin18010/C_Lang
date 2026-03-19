/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\04_Microarchitecture_Benchmarking.md
 * Line: 974
 * Language: c
 * Block ID: c71489a4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* tlb_test.c - TLB容量和延迟测试 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>

/* RDTSC */
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

#define PAGE_SIZE 4096
#define NUM_PAGES 8192  /* 测试32MB */
#define ITERATIONS 1000000

/* 测试TLB访问延迟 */
void test_tlb_latency(void) {
    printf("========================================\n");
    printf("      TLB容量测试                       \n");
    printf("========================================\n\n");

    /* 分配页数组 */
    char **pages = malloc(NUM_PAGES * sizeof(char*));

    /* 分配内存页 */
    for (int i = 0; i < NUM_PAGES; i++) {
        pages[i] = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (pages[i] == MAP_FAILED) {
            perror("mmap");
            return;
        }
        /* 写入数据，确保页被分配 */
        pages[i][0] = i;
    }

    /* 创建不同TLB覆盖范围的测试 */
    int test_sizes[] = {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};
    int num_tests = sizeof(test_sizes) / sizeof(test_sizes[0]);

    printf("页数      覆盖大小      访问延迟(cycles)      注释\n");
    printf("----------------------------------------------------------------\n");

    for (int t = 0; t < num_tests; t++) {
        int num_pages = test_sizes[t];

        /* 创建随机访问序列 */
        int *sequence = malloc(num_pages * sizeof(int));
        for (int i = 0; i < num_pages; i++) {
            sequence[i] = i;
        }

        /* 打乱顺序 */
        for (int i = num_pages - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            int tmp = sequence[i];
            sequence[i] = sequence[j];
            sequence[j] = tmp;
        }

        /* 预热TLB */
        for (int i = 0; i < num_pages; i++) {
            volatile char c = pages[sequence[i]][0];
            (void)c;
        }

        /* 测量访问时间 */
        uint32_t aux;
        uint64_t start = rdtscp(&aux);

        volatile char sum = 0;
        for (int iter = 0; iter < ITERATIONS; iter++) {
            for (int i = 0; i < num_pages; i++) {
                sum += pages[sequence[i]][0];
            }
        }

        uint64_t end = rdtscp(&aux);
        (void)sum;

        double avg_cycles = (double)(end - start) / (ITERATIONS * num_pages);

        /* 判断TLB状态 */
        const char *note = "";
        if (avg_cycles < 10) {
            note = "(TLB命中)";
        } else if (avg_cycles < 50) {
            note = "(L1 TLB未命中)";
        } else {
            note = "(L2 TLB/页表遍历)";
        }

        printf("%-9d %-13s %18.2f      %s\n",
               num_pages,
               num_pages <= 4 ? "<64KB" :
               num_pages <= 64 ? "<256KB" :
               num_pages <= 512 ? "<2MB" :
               num_pages <= 2048 ? "<8MB" : ">32MB",
               avg_cycles,
               note);

        free(sequence);
    }

    /* 清理 */
    for (int i = 0; i < NUM_PAGES; i++) {
        munmap(pages[i], PAGE_SIZE);
    }
    free(pages);

    printf("\n========================================\n");
    printf("典型TLB配置:\n");
    printf("  L1 DTLB: 64-entries (覆盖256KB with 4KB pages)\n");
    printf("  L2 TLB:  1024-entries (覆盖4MB with 4KB pages)\n");
    printf("  使用2MB大页可显著增加覆盖范围\n");
    printf("========================================\n");
}

/* 大页TLB测试 */
void test_hugepage_tlb(void) {
    printf("\n========================================\n");
    printf("      大页TLB测试                       \n");
    printf("========================================\n\n");

    /* 尝试分配2MB大页 */
    size_t hugepage_size = 2 * 1024 * 1024;  /* 2MB */
    int num_hugepages = 32;  /* 64MB */

    char **hugepages = malloc(num_hugepages * sizeof(char*));

    printf("分配 %d x 2MB 大页...\n", num_hugepages);

    for (int i = 0; i < num_hugepages; i++) {
        hugepages[i] = mmap(NULL, hugepage_size, PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
                           -1, 0);
        if (hugepages[i] == MAP_FAILED) {
            printf("大页分配失败 (可能需要配置hugetlbfs)\n");
            free(hugepages);
            return;
        }
        hugepages[i][0] = i;
    }

    printf("大页分配成功!\n\n");

    /* 测试大页TLB访问 */
    int test_sizes[] = {4, 8, 16, 32};

    printf("大页数    覆盖大小      访问延迟(cycles)\n");
    printf("----------------------------------------------------------------\n");

    for (int t = 0; t < sizeof(test_sizes)/sizeof(test_sizes[0]); t++) {
        int num = test_sizes[t];

        /* 随机顺序 */
        int *seq = malloc(num * sizeof(int));
        for (int i = 0; i < num; i++) seq[i] = i;
        for (int i = num-1; i > 0; i--) {
            int j = rand() % (i+1);
            int tmp = seq[i]; seq[i] = seq[j]; seq[j] = tmp;
        }

        /* 测量 */
        uint32_t aux;
        volatile char sum = 0;

        uint64_t start = rdtscp(&aux);
        for (int iter = 0; iter < 100000; iter++) {
            for (int i = 0; i < num; i++) {
                sum += hugepages[seq[i]][0];
            }
        }
        uint64_t end = rdtscp(&aux);
        (void)sum;

        double avg = (double)(end - start) / (100000 * num);

        char size_str[32];
        snprintf(size_str, sizeof(size_str), "%dMB", num * 2);

        printf("%-9d %-13s %18.2f\n", num, size_str, avg);

        free(seq);
    }

    /* 清理 */
    for (int i = 0; i < num_hugepages; i++) {
        munmap(hugepages[i], hugepage_size);
    }
    free(hugepages);
}

int main() {
    srand(time(NULL));

    test_tlb_latency();
    test_hugepage_tlb();

    return 0;
}
