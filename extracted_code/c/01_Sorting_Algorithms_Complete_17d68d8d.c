/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\01_Sorting_Algorithms_Complete.md
 * Line: 398
 * Language: c
 * Block ID: 17d68d8d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 选择排序
 * 文件名: selection_sort.c
 * 编译: gcc -std=c23 -o selection_sort selection_sort.c
 */

#include <stdio.h>
#include <stdlib.h>

static inline void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * @brief 基础选择排序
 * @param arr 待排序数组
 * @param n 数组长度
 */
void selection_sort(int arr[], size_t n) {
    for (size_t i = 0; i < n - 1; i++) {
        size_t min_idx = i;

        // 在未排序部分找最小值
        for (size_t j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }

        // 将最小值放到已排序部分的末尾
        if (min_idx != i) {
            swap(&arr[i], &arr[min_idx]);
        }
    }
}

/**
 * @brief 双向选择排序 - 每次找最大和最小
 * @param arr 待排序数组
 * @param n 数组长度
 */
void selection_sort_bidirectional(int arr[], size_t n) {
    size_t left = 0;
    size_t right = n - 1;

    while (left < right) {
        size_t min_idx = left;
        size_t max_idx = left;

        // 在[left, right]范围内找最小和最大
        for (size_t i = left; i <= right; i++) {
            if (arr[i] < arr[min_idx]) min_idx = i;
            if (arr[i] > arr[max_idx]) max_idx = i;
        }

        // 将最小值放到左边
        swap(&arr[left], &arr[min_idx]);

        // 如果最大值在left位置，交换后它到了min_idx位置
        if (max_idx == left) {
            max_idx = min_idx;
        }

        // 将最大值放到右边
        swap(&arr[right], &arr[max_idx]);

        left++;
        right--;
    }
}

/**
 * @brief 递归选择排序
 * @param arr 待排序数组
 * @param n 数组长度
 */
void selection_sort_recursive(int arr[], size_t n) {
    if (n <= 1) return;

    // 找最大值
    size_t max_idx = 0;
    for (size_t i = 1; i < n; i++) {
        if (arr[i] > arr[max_idx]) {
            max_idx = i;
        }
    }

    // 将最大值放到最后
    swap(&arr[max_idx], &arr[n - 1]);

    // 递归排序剩余部分
    selection_sort_recursive(arr, n - 1);
}

void print_array(const int arr[], size_t n) {
    printf("[");
    for (size_t i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

int main(void) {
    int arr1[] = {64, 25, 12, 22, 11, 90, 5};
    int arr2[] = {64, 25, 12, 22, 11, 90, 5};
    size_t n = sizeof(arr1) / sizeof(arr1[0]);

    printf("=== 选择排序演示 ===\n");
    printf("原始数组: ");
    print_array(arr1, n);

    selection_sort(arr1, n);
    printf("基础选择: ");
    print_array(arr1, n);

    selection_sort_bidirectional(arr2, n);
    printf("双向选择: ");
    print_array(arr2, n);

    return 0;
}
