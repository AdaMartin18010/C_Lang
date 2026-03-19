/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 2241
 * Language: c
 * Block ID: 1b915deb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file hugepages_demo.c
 * @brief HugePages大页内存优化
 *
 * 大页内存优势：
 * - 减少TLB miss（每页更大，同样内存需要更少的页表项）
 * - 减少页表遍历开销
 * - 适用于大内存工作集应用
 *
 * 标准页：4KB，x86 HugePages：2MB 或 1GB
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

/* 检测大页支持 */
void check_hugepage_support(void) {
    printf("=== HugePage Support ===\n\n");

    long default_size = sysconf(_SC_PAGESIZE);
    printf("Default page size: %ld KB\n", default_size / 1024);

    /* 检查/proc/meminfo中的大页信息 */
    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, "HugePages") || strstr(line, "Hugepagesize")) {
                printf("%s", line);
            }
        }
        fclose(fp);
    }
}

/**
 * @brief 使用mmap分配大页内存
 */
void* allocate_hugepage(size_t size) {
    /* MAP_HUGETLB: 请求2MB大页 */
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);

    if (ptr == MAP_FAILED) {
        perror("mmap hugepage failed");
        return NULL;
    }

    return ptr;
}

/**
 * @brief 透明大页(THP)分配
 *
 * Linux透明大页自动将普通分配合并为大页
 * 通过madvise提示内核使用THP
 */
void* allocate_thp(size_t size) {
    /* 标准分配 */
    void *ptr = aligned_alloc(2 * 1024 * 1024, size);  /* 2MB对齐 */
    if (!ptr) return NULL;

    /* 提示内核使用透明大页 */
    madvise(ptr, size, MADV_HUGEPAGE);

    return ptr;
}

/**
 * @brief 内存访问性能测试
 */
void memory_access_test(void *memory, size_t size, const char *label) {
    volatile long long sum = 0;
    int *arr = (int*)memory;
    int num_elements = size / sizeof(int);

    /* 初始化 */
    for (int i = 0; i < num_elements; i++) {
        arr[i] = i;
    }

    /* 顺序访问测试 */
    clock_t start = clock();
    for (int iter = 0; iter < 10; iter++) {
        for (int i = 0; i < num_elements; i++) {
            sum += arr[i];
        }
    }
    clock_t end = clock();

    double ms = (end - start) * 1000.0 / CLOCKS_PER_SEC;
    double bandwidth = (10.0 * size / (1024 * 1024)) / (ms / 1000.0);

    printf("%s: %.2f ms, %.2f MB/s (sum=%lld)\n",
           label, ms, bandwidth, sum);
}

void benchmark_hugepages(void) {
    printf("\n=== HugePage Performance Test ===\n\n");

    const size_t SIZE = 512 * 1024 * 1024;  /* 512MB */

    /* 标准内存 */
    void *normal = aligned_alloc(4096, SIZE);
    if (normal) {
        memory_access_test(normal, SIZE, "Normal pages (4KB)");
        free(normal);
    }

    /* 透明大页 */
    void *thp = allocate_thp(SIZE);
    if (thp) {
        memory_access_test(thp, SIZE, "Transparent HugePage");
        free(thp);
    }

    /* 显式大页 - 需要root权限和预留 */
    void *huge = allocate_hugepage(SIZE);
    if (huge) {
        memory_access_test(huge, SIZE, "Explicit HugePage (2MB)");
        munmap(huge, SIZE);
    }
}

/**
 * @brief TLB性能分析
 *
 * TLB (Translation Lookaside Buffer)缓存虚拟到物理地址映射
 * 大页减少TLB条目数量，提高TLB命中率
 */
void tlb_analysis(void) {
    printf("\n=== TLB Analysis ===\n\n");

    /* 计算不同页大小的TLB条目需求 */
    size_t memory_size = 1 * 1024 * 1024 * 1024;  /* 1GB */

    printf("Memory size: 1GB\n");
    printf("4KB pages:   %zu TLB entries needed\n", memory_size / (4 * 1024));
    printf("2MB pages:   %zu TLB entries needed\n", memory_size / (2 * 1024 * 1024));
    printf("1GB pages:   %zu TLB entries needed\n", memory_size / (1024 * 1024 * 1024));

    printf("\nTypical TLB sizes:\n");
    printf("L1 dTLB: 64 entries (4KB), 32 entries (2MB)\n");
    printf("L2 sTLB: 1024+ entries\n");
    printf("\nWith 4KB pages and 1GB memory: %zu TLB misses expected\n",
           memory_size / (4 * 1024) / 64);
    printf("With 2MB pages and 1GB memory: %zu TLB misses expected\n",
           memory_size / (2 * 1024 * 1024) / 32);
}

int main(void) {
    check_hugepage_support();
    benchmark_hugepages();
    tlb_analysis();
    return 0;
}
