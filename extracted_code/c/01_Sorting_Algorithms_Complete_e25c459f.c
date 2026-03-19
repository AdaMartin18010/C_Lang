/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\01_Sorting_Algorithms_Complete.md
 * Line: 571
 * Language: c
 * Block ID: e25c459f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 插入排序
 * 文件名: insertion_sort.c
 * 编译: gcc -std=c23 -o insertion_sort insertion_sort.c
 */

#include <stdio.h>
#include <string.h>

/**
 * @brief 基础插入排序
 * @param arr 待排序数组
 * @param n 数组长度
 */
void insertion_sort(int arr[], size_t n) {
    for (size_t i = 1; i < n; i++) {
        int key = arr[i];
        int j = (int)i - 1;

        // 将大于key的元素后移
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }

        arr[j + 1] = key;
    }
}

/**
 * @brief 二分插入排序 - 使用二分查找找插入位置
 * @param arr 待排序数组
 * @param n 数组长度
 */
void binary_insertion_sort(int arr[], size_t n) {
    for (size_t i = 1; i < n; i++) {
        int key = arr[i];

        // 二分查找插入位置
        int left = 0;
        int right = (int)i - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (arr[mid] > key) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }

        // 后移元素
        for (int j = (int)i - 1; j >= left; j--) {
            arr[j + 1] = arr[j];
        }

        arr[left] = key;
    }
}

/**
 * @brief 希尔排序 - 插入排序的改进
 * @param arr 待排序数组
 * @param n 数组长度
 */
void shell_sort(int arr[], size_t n) {
    // 使用Knuth序列: 1, 4, 13, 40, 121, ...
    size_t gap = 1;
    while (gap < n / 3) {
        gap = gap * 3 + 1;
    }

    while (gap >= 1) {
        // 对每个子数组进行插入排序
        for (size_t i = gap; i < n; i++) {
            int key = arr[i];
            int j = (int)i;

            while (j >= (int)gap && arr[j - gap] > key) {
                arr[j] = arr[j - gap];
                j -= (int)gap;
            }

            arr[j] = key;
        }

        gap /= 3;
    }
}

/**
 * @brief 链表插入排序
 * @param arr 待排序数组
 * @param n 数组长度
 * @note 使用链表思想优化大量数据移动
 */
void insertion_sort_linked_list_style(int arr[], size_t n) {
    if (n <= 1) return;

    // 创建索引数组，模拟链表
    int *next = malloc(n * sizeof(int));
    if (!next) return;

    // 初始化链表头
    int head = 0;
    next[0] = -1;

    // 逐个插入到链表
    for (size_t i = 1; i < n; i++) {
        int key = arr[i];

        // 找插入位置
        int prev = -1;
        int curr = head;

        while (curr != -1 && arr[curr] <= key) {
            prev = curr;
            curr = next[curr];
        }

        // 插入
        if (prev == -1) {
            next[i] = head;
            head = (int)i;
        } else {
            next[i] = next[prev];
            next[prev] = (int)i;
        }
    }

    // 从链表重建数组
    int curr = head;
    size_t idx = 0;
    int *temp = malloc(n * sizeof(int));

    while (curr != -1) {
        temp[idx++] = arr[curr];
        curr = next[curr];
    }

    memcpy(arr, temp, n * sizeof(int));

    free(next);
    free(temp);
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
    int arr1[] = {64, 34, 25, 12, 22, 11, 90};
    int arr2[] = {64, 34, 25, 12, 22, 11, 90};
    int arr3[] = {64, 34, 25, 12, 22, 11, 90};
    size_t n = sizeof(arr1) / sizeof(arr1[0]);

    printf("=== 插入排序演示 ===\n");
    printf("原始数组: ");
    print_array(arr1, n);

    insertion_sort(arr1, n);
    printf("基础插入: ");
    print_array(arr1, n);

    binary_insertion_sort(arr2, n);
    printf("二分插入: ");
    print_array(arr2, n);

    shell_sort(arr3, n);
    printf("希尔排序: ");
    print_array(arr3, n);

    return 0;
}
