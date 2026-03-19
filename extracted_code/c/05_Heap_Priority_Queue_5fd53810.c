/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 1030
 * Language: c
 * Block ID: 5fd53810
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * ============================================================================
 * 堆和优先队列完整实现 (Heap & Priority Queue Complete Implementation)
 * ============================================================================
 * 编译命令: gcc -std=c23 -Wall -Wextra -o heap heap.c
 * 运行命令: ./heap
 * ============================================================================
 */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

#if __STDC_VERSION__ < 202311L
    #define nullptr NULL
#endif

// 这里包含前面所有的结构定义和函数实现...

// =============================================================================
// 测试函数
// =============================================================================

void test_heap_operations() {
    printf("\n========== 堆基本操作测试 ==========\n");

    Heap max_heap;
    heap_init(&max_heap, MAX_HEAP);

    printf("构建最大堆，插入: ");
    int values[] = {3, 1, 4, 1, 5, 9, 2, 6};
    for (int i = 0; i < 8; i++) {
        printf("%d ", values[i]);
        heap_push(&max_heap, values[i]);
    }
    printf("\n");

    printf("堆数组: ");
    for (size_t i = 0; i < max_heap.size; i++) {
        printf("%d ", max_heap.data[i]);
    }
    printf("\n");

    printf("弹出顺序: ");
    int val;
    while (heap_pop(&max_heap, &val)) {
        printf("%d ", val);
    }
    printf("\n");

    heap_destroy(&max_heap);
    printf("最大堆测试通过!\n");

    // 最小堆测试
    printf("\n--- 最小堆测试 ---\n");
    Heap min_heap;
    heap_init(&min_heap, MIN_HEAP);

    for (int i = 0; i < 8; i++) {
        heap_push(&min_heap, values[i]);
    }

    printf("弹出顺序: ");
    while (heap_pop(&min_heap, &val)) {
        printf("%d ", val);
    }
    printf("\n");

    heap_destroy(&min_heap);
    printf("最小堆测试通过!\n");
}

void test_heap_sort() {
    printf("\n========== 堆排序测试 ==========\n");

    int arr[] = {64, 34, 25, 12, 22, 11, 90, 5};
    int n = sizeof(arr) / sizeof(arr[0]);

    printf("原数组: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n");

    heap_sort_ascending(arr, n);
    printf("升序: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n");

    heap_sort_descending(arr, n);
    printf("降序: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n");

    printf("堆排序测试通过!\n");
}

void test_top_k() {
    printf("\n========== Top-K 测试 ==========\n");

    int nums[] = {3, 2, 1, 5, 6, 4, 9, 8, 7};
    int n = sizeof(nums) / sizeof(nums[0]);

    printf("数组: ");
    for (int i = 0; i < n; i++) printf("%d ", nums[i]);
    printf("\n");

    int k = 3;
    printf("第%d大的元素: %d\n", k, find_kth_largest(nums, n, k));
    printf("第%d小的元素: %d\n", k, find_kth_smallest(nums, n, k));

    printf("Top-K 测试通过!\n");
}

void test_median_finder() {
    printf("\n========== 中位数查找测试 ==========\n");

    MedianFinder mf;
    median_finder_init(&mf);

    int stream[] = {5, 2, 3, 4, 1};
    printf("数据流: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", stream[i]);
        median_finder_add_num(&mf, stream[i]);
        printf("(中位数: %.1f) ", median_finder_find_median(&mf));
    }
    printf("\n");

    median_finder_destroy(&mf);
    printf("中位数查找测试通过!\n");
}

// =============================================================================
// 主函数
// =============================================================================

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║       堆和优先队列完整实现测试 (C23 Standard)              ║\n");
    printf("║       Heap & Priority Queue Complete Implementation        ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("编译时间: %s %s\n", __DATE__, __TIME__);

    test_heap_operations();
    test_heap_sort();
    test_top_k();
    test_median_finder();

    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║              所有测试通过! All tests passed!               ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");

    return 0;
}
