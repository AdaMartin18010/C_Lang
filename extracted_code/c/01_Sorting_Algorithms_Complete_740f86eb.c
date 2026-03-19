/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\01_Sorting_Algorithms_Complete.md
 * Line: 1740
 * Language: c
 * Block ID: 740f86eb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 计数排序
 * 文件名: counting_sort.c
 * 编译: gcc -std=c23 -o counting_sort counting_sort.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 基础计数排序
 * @param arr 输入数组
 * @param n 数组长度
 * @param max_val 数组中最大值
 * @param min_val 数组中最小值
 */
void counting_sort_basic(int arr[], size_t n, int min_val, int max_val) {
    if (n <= 1) return;

    int range = max_val - min_val + 1;
    int *count = calloc(range, sizeof(int));
    int *output = malloc(n * sizeof(int));

    if (!count || !output) {
        free(count);
        free(output);
        return;
    }

    // 统计每个值出现的次数
    for (size_t i = 0; i < n; i++) {
        count[arr[i] - min_val]++;
    }

    // 计算累积计数
    for (int i = 1; i < range; i++) {
        count[i] += count[i - 1];
    }

    // 从后向前放置元素（保持稳定）
    for (int i = (int)n - 1; i >= 0; i--) {
        output[count[arr[i] - min_val] - 1] = arr[i];
        count[arr[i] - min_val]--;
    }

    // 复制回原始数组
    memcpy(arr, output, n * sizeof(int));

    free(count);
    free(output);
}

/**
 * @brief 原地计数排序 - 需要额外空间但原地修改
 */
void counting_sort_inplace(int arr[], size_t n, int min_val, int max_val) {
    int range = max_val - min_val + 1;
    int *count = calloc(range, sizeof(int));

    if (!count) return;

    // 统计
    for (size_t i = 0; i < n; i++) {
        count[arr[i] - min_val]++;
    }

    // 重写数组
    size_t idx = 0;
    for (int i = 0; i < range; i++) {
        while (count[i]-- > 0) {
            arr[idx++] = i + min_val;
        }
    }

    free(count);
}

/**
 * @brief 优化的计数排序 - 自动计算范围
 */
void counting_sort_auto(int arr[], size_t n) {
    if (n <= 1) return;

    // 找最小最大值
    int min_val = arr[0];
    int max_val = arr[0];

    for (size_t i = 1; i < n; i++) {
        if (arr[i] < min_val) min_val = arr[i];
        if (arr[i] > max_val) max_val = arr[i];
    }

    counting_sort_basic(arr, n, min_val, max_val);
}

/**
 * @brief 计数排序排序结构体数组
 */
typedef struct {
    int key;
    char data[32];
} Record;

void counting_sort_records(Record arr[], size_t n, int min_key, int max_key) {
    int range = max_key - min_key + 1;
    int *count = calloc(range, sizeof(int));
    Record *output = malloc(n * sizeof(Record));

    if (!count || !output) {
        free(count);
        free(output);
        return;
    }

    for (size_t i = 0; i < n; i++) {
        count[arr[i].key - min_key]++;
    }

    for (int i = 1; i < range; i++) {
        count[i] += count[i - 1];
    }

    for (int i = (int)n - 1; i >= 0; i--) {
        output[count[arr[i].key - min_key] - 1] = arr[i];
        count[arr[i].key - min_key]--;
    }

    memcpy(arr, output, n * sizeof(Record));

    free(count);
    free(output);
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
    int arr[] = {4, 2, 2, 8, 3, 3, 1, 7, 5, 6};
    size_t n = sizeof(arr) / sizeof(arr[0]);

    printf("=== 计数排序演示 ===\n");
    printf("原始数组: ");
    print_array(arr, n);

    counting_sort_auto(arr, n);
    printf("排序结果: ");
    print_array(arr, n);

    return 0;
}
