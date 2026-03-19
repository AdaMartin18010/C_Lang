/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\01_PMU_Architecture.md
 * Line: 2142
 * Language: c
 * Block ID: f29aee19
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* branch_analysis.c - 分支预测性能分析案例 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

/* 完全可预测的分支 */
void predictable_branches(int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        /* 总是为真，完全可预测 */
        if (i >= 0) {
            sum++;
        }
    }
}

/* 随机分支 */
void random_branches(int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        if (rand() % 2) {
            sum++;
        }
    }
}

/* 交替分支 (最难预测) */
void alternating_branches(int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        if (i & 1) {
            sum++;
        }
    }
}

/* 基于数据的分支 */
void data_dependent_branches(int *data, int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        if (data[i] > 100) {
            sum += data[i];
        }
    }
}

/* 排序后数据的分支 */
void sorted_data_branches(int *data, int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        if (data[i] > 100) {
            sum += data[i];
        }
    }
}

/* 二分查找 */
int binary_search(int *arr, int n, int target) {
    int left = 0, right = n - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] == target) {
            return mid;
        } else if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

/* 线性查找 */
int linear_search(int *arr, int n, int target) {
    for (int i = 0; i < n; i++) {
        if (arr[i] == target) {
            return i;
        }
    }
    return -1;
}

/* 计算分支预测准确率 */
void calculate_branch_accuracy(uint64_t branches, uint64_t misses) {
    if (branches > 0) {
        double accuracy = 100.0 * (1.0 - (double)misses / branches);
        printf("  分支预测准确率: %.2f%%\n", accuracy);
        printf("  预测失败率: %.2f%%\n", 100.0 - accuracy);
    }
}

int main(void) {
    struct perf_event_attr attr_branch, attr_miss;
    int fd_branch, fd_miss;
    uint64_t branches, misses;

    const int N = 100000000;
    const int SEARCH_N = 1000000;

    printf("========== 分支预测性能分析案例 ==========\n\n");

    /* 配置分支事件 */
    memset(&attr_branch, 0, sizeof(attr_branch));
    attr_branch.type = PERF_TYPE_HARDWARE;
    attr_branch.config = PERF_COUNT_HW_BRANCH_INSTRUCTIONS;
    attr_branch.size = sizeof(attr_branch);
    attr_branch.disabled = 1;
    attr_branch.exclude_kernel = 1;

    memset(&attr_miss, 0, sizeof(attr_miss));
    attr_miss.type = PERF_TYPE_HARDWARE;
    attr_miss.config = PERF_COUNT_HW_BRANCH_MISSES;
    attr_miss.size = sizeof(attr_miss);
    attr_miss.disabled = 1;
    attr_miss.exclude_kernel = 1;

    /* 测试1: 可预测分支 */
    printf("测试1: 完全可预测分支\n");
    printf("----------------------\n");

    fd_branch = perf_event_open(&attr_branch, 0, -1, -1, 0);
    fd_miss = perf_event_open(&attr_miss, 0, -1, fd_branch, 0);

    ioctl(fd_branch, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_branch, PERF_EVENT_IOC_ENABLE, 0);
    predictable_branches(N);
    ioctl(fd_branch, PERF_EVENT_IOC_DISABLE, 0);

    read(fd_branch, &branches, sizeof(branches));
    read(fd_miss, &misses, sizeof(misses));

    printf("分支指令数: %" PRIu64 "\n", branches);
    printf("分支预测失败: %" PRIu64 "\n", misses);
    calculate_branch_accuracy(branches, misses);

    close(fd_branch);
    close(fd_miss);

    /* 测试2: 随机分支 */
    printf("\n测试2: 随机分支\n");
    printf("---------------\n");

    srand(time(NULL));
    fd_branch = perf_event_open(&attr_branch, 0, -1, -1, 0);
    fd_miss = perf_event_open(&attr_miss, 0, -1, fd_branch, 0);

    ioctl(fd_branch, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_branch, PERF_EVENT_IOC_ENABLE, 0);
    random_branches(N);
    ioctl(fd_branch, PERF_EVENT_IOC_DISABLE, 0);

    read(fd_branch, &branches, sizeof(branches));
    read(fd_miss, &misses, sizeof(misses));

    printf("分支指令数: %" PRIu64 "\n", branches);
    printf("分支预测失败: %" PRIu64 "\n", misses);
    calculate_branch_accuracy(branches, misses);

    close(fd_branch);
    close(fd_miss);

    /* 测试3: 数据依赖分支 (排序优化) */
    printf("\n测试3: 数据依赖分支优化\n");
    printf("-----------------------\n");

    int *data = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) {
        data[i] = rand() % 200;
    }

    /* 未排序数据 */
    fd_branch = perf_event_open(&attr_branch, 0, -1, -1, 0);
    fd_miss = perf_event_open(&attr_miss, 0, -1, fd_branch, 0);

    ioctl(fd_branch, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_branch, PERF_EVENT_IOC_ENABLE, 0);
    data_dependent_branches(data, N);
    ioctl(fd_branch, PERF_EVENT_IOC_DISABLE, 0);

    read(fd_branch, &branches, sizeof(branches));
    read(fd_miss, &misses, sizeof(misses));

    printf("未排序数据:\n");
    printf("  分支指令数: %" PRIu64 "\n", branches);
    printf("  分支预测失败: %" PRIu64 "\n", misses);
    calculate_branch_accuracy(branches, misses);

    close(fd_branch);
    close(fd_miss);

    /* 排序后数据 */
    /* 排序使数据具有模式，提高分支预测准确率 */
    qsort(data, N, sizeof(int),
          (int (*)(const void*, const void*))strcmp);

    fd_branch = perf_event_open(&attr_branch, 0, -1, -1, 0);
    fd_miss = perf_event_open(&attr_miss, 0, -1, fd_branch, 0);

    ioctl(fd_branch, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_branch, PERF_EVENT_IOC_ENABLE, 0);
    sorted_data_branches(data, N);
    ioctl(fd_branch, PERF_EVENT_IOC_DISABLE, 0);

    read(fd_branch, &branches, sizeof(branches));
    read(fd_miss, &misses, sizeof(misses));

    printf("排序后数据:\n");
    printf("  分支指令数: %" PRIu64 "\n", branches);
    printf("  分支预测失败: %" PRIu64 "\n", misses);
    calculate_branch_accuracy(branches, misses);

    close(fd_branch);
    close(fd_miss);

    /* 测试4: 查找算法对比 */
    printf("\n测试4: 查找算法分支行为\n");
    printf("-----------------------\n");

    int *search_arr = malloc(SEARCH_N * sizeof(int));
    for (int i = 0; i < SEARCH_N; i++) {
        search_arr[i] = i * 2;
    }

    /* 线性查找 */
    fd_branch = perf_event_open(&attr_branch, 0, -1, -1, 0);
    fd_miss = perf_event_open(&attr_miss, 0, -1, fd_branch, 0);

    ioctl(fd_branch, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_branch, PERF_EVENT_IOC_ENABLE, 0);
    for (int i = 0; i < 1000; i++) {
        linear_search(search_arr, SEARCH_N, i * 100);
    }
    ioctl(fd_branch, PERF_EVENT_IOC_DISABLE, 0);

    read(fd_branch, &branches, sizeof(branches));
    read(fd_miss, &misses, sizeof(misses));

    printf("线性查找:\n");
    printf("  分支指令数: %" PRIu64 "\n", branches);
    printf("  分支预测失败: %" PRIu64 "\n", misses);
    calculate_branch_accuracy(branches, misses);

    close(fd_branch);
    close(fd_miss);

    /* 二分查找 */
    fd_branch = perf_event_open(&attr_branch, 0, -1, -1, 0);
    fd_miss = perf_event_open(&attr_miss, 0, -1, fd_branch, 0);

    ioctl(fd_branch, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_branch, PERF_EVENT_IOC_ENABLE, 0);
    for (int i = 0; i < 1000; i++) {
        binary_search(search_arr, SEARCH_N, i * 100);
    }
    ioctl(fd_branch, PERF_EVENT_IOC_DISABLE, 0);

    read(fd_branch, &branches, sizeof(branches));
    read(fd_miss, &misses, sizeof(misses));

    printf("二分查找:\n");
    printf("  分支指令数: %" PRIu64 "\n", branches);
    printf("  分支预测失败: %" PRIu64 "\n", misses);
    calculate_branch_accuracy(branches, misses);

    close(fd_branch);
    close(fd_miss);

    free(data);
    free(search_arr);

    printf("\n========== 分析完成 ==========\n");
    printf("\n优化建议:\n");
    printf("1. 尽量减少随机分支模式\n");
    printf("2. 对数据排序可提高分支预测准确率\n");
    printf("3. 考虑使用位运算替代条件分支\n");
    printf("4. 使用likely/unlikely提示编译器\n");

    return 0;
}
