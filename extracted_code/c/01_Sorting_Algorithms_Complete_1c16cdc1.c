/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\01_Sorting_Algorithms_Complete.md
 * Line: 229
 * Language: c
 * Block ID: 1c16cdc1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 冒泡排序 - 基础版本
 * 文件名: bubble_sort_basic.c
 * 编译: gcc -std=c23 -o bubble_sort_basic bubble_sort_basic.c
 */

#include <stdio.h>
#include <stdbool.h>

// 交换两个整数
static inline void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * @brief 基础冒泡排序
 * @param arr 待排序数组
 * @param n 数组长度
 */
void bubble_sort_basic(int arr[], size_t n) {
    for (size_t i = 0; i < n - 1; i++) {
        for (size_t j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
            }
        }
    }
}

/**
 * @brief 优化冒泡排序 - 添加提前退出
 * @param arr 待排序数组
 * @param n 数组长度
 */
void bubble_sort_optimized(int arr[], size_t n) {
    for (size_t i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (size_t j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
                swapped = true;
            }
        }
        // 如果没有发生交换，说明数组已有序
        if (!swapped) break;
    }
}

/**
 * @brief 鸡尾酒排序（双向冒泡排序）
 * @param arr 待排序数组
 * @param n 数组长度
 */
void cocktail_sort(int arr[], size_t n) {
    bool swapped = true;
    size_t start = 0;
    size_t end = n - 1;

    while (swapped) {
        swapped = false;

        // 正向遍历
        for (size_t i = start; i < end; i++) {
            if (arr[i] > arr[i + 1]) {
                swap(&arr[i], &arr[i + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;

        end--;
        swapped = false;

        // 反向遍历
        for (size_t i = end; i > start; i--) {
            if (arr[i - 1] > arr[i]) {
                swap(&arr[i - 1], &arr[i]);
                swapped = true;
            }
        }
        start++;
    }
}

// 打印数组
void print_array(const int arr[], size_t n) {
    printf("[");
    for (size_t i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

int main(void) {
    int arr1[] = {64, 34, 25, 12, 22, 11, 90};
    int arr2[] = {64, 34, 25, 12, 22, 11, 90};
    int arr3[] = {64, 34, 25, 12, 22, 11, 90};
    size_t n = sizeof(arr1) / sizeof(arr1[0]);

    printf("=== 冒泡排序演示 ===\n");
    printf("原始数组: ");
    print_array(arr1, n);

    bubble_sort_basic(arr1, n);
    printf("基础冒泡: ");
    print_array(arr1, n);

    bubble_sort_optimized(arr2, n);
    printf("优化冒泡: ");
    print_array(arr2, n);

    cocktail_sort(arr3, n);
    printf("鸡尾酒排序: ");
    print_array(arr3, n);

    return 0;
}
