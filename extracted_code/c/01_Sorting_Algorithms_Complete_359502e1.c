/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\01_Sorting_Algorithms_Complete.md
 * Line: 2232
 * Language: c
 * Block ID: 359502e1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 基数排序
 * 文件名: radix_sort.c
 * 编译: gcc -std=c23 -o radix_sort radix_sort.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 计数排序子程序 - 按特定位排序
 * @param arr 输入数组
 * @param n 数组长度
 * @param exp 当前位（1, 10, 100, ...）
 */
void count_sort_for_radix(int arr[], size_t n, int exp) {
    int *output = malloc(n * sizeof(int));
    int count[10] = {0};

    if (!output) return;

    // 统计当前位数字出现次数
    for (size_t i = 0; i < n; i++) {
        count[(arr[i] / exp) % 10]++;
    }

    // 累积计数
    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }

    // 从后向前构建输出数组（保持稳定性）
    for (int i = (int)n - 1; i >= 0; i--) {
        int digit = (arr[i] / exp) % 10;
        output[count[digit] - 1] = arr[i];
        count[digit]--;
    }

    // 复制回原数组
    memcpy(arr, output, n * sizeof(int));
    free(output);
}

/**
 * @brief LSD基数排序（最低位优先）
 */
void radix_sort_lsd(int arr[], size_t n) {
    if (n <= 1) return;

    // 找最大值确定位数
    int max_val = arr[0];
    for (size_t i = 1; i < n; i++) {
        if (arr[i] > max_val) max_val = arr[i];
    }

    // 对每一位进行计数排序
    for (int exp = 1; max_val / exp > 0; exp *= 10) {
        count_sort_for_radix(arr, n, exp);
    }
}

/**
 * @brief MSD基数排序（最高位优先）- 递归实现
 */
void radix_sort_msd_helper(int arr[], size_t n, int exp);

void radix_sort_msd(int arr[], size_t n) {
    if (n <= 1) return;

    int max_val = arr[0];
    for (size_t i = 1; i < n; i++) {
        if (arr[i] > max_val) max_val = arr[i];
    }

    // 找到最高位的exp
    int exp = 1;
    while (max_val / exp >= 10) {
        exp *= 10;
    }

    radix_sort_msd_helper(arr, n, exp);
}

void radix_sort_msd_helper(int arr[], size_t n, int exp) {
    if (n <= 1 || exp == 0) return;

    // 按当前位分组
    int *buckets[10];
    size_t bucket_sizes[10] = {0};

    // 统计每桶大小
    for (size_t i = 0; i < n; i++) {
        int digit = (arr[i] / exp) % 10;
        bucket_sizes[digit]++;
    }

    // 分配内存
    for (int i = 0; i < 10; i++) {
        buckets[i] = malloc(bucket_sizes[i] * sizeof(int));
        bucket_sizes[i] = 0;  // 重置用于索引
    }

    // 放入桶中
    for (size_t i = 0; i < n; i++) {
        int digit = (arr[i] / exp) % 10;
        buckets[digit][bucket_sizes[digit]++] = arr[i];
    }

    // 递归排序每个非空桶
    size_t idx = 0;
    for (int i = 0; i < 10; i++) {
        if (bucket_sizes[i] > 1) {
            radix_sort_msd_helper(buckets[i], bucket_sizes[i], exp / 10);
        }
        // 复制回数组
        for (size_t j = 0; j < bucket_sizes[i]; j++) {
            arr[idx++] = buckets[i][j];
        }
        free(buckets[i]);
    }
}

/**
 * @brief 基数排序处理负数
 */
void radix_sort_with_negatives(int arr[], size_t n) {
    if (n <= 1) return;

    // 分离负数和正数
    int *negatives = malloc(n * sizeof(int));
    int *positives = malloc(n * sizeof(int));
    size_t neg_count = 0, pos_count = 0;

    for (size_t i = 0; i < n; i++) {
        if (arr[i] < 0) {
            negatives[neg_count++] = -arr[i];  // 取反
        } else {
            positives[pos_count++] = arr[i];
        }
    }

    // 分别排序
    if (neg_count > 0) {
        radix_sort_lsd(negatives, neg_count);
    }
    if (pos_count > 0) {
        radix_sort_lsd(positives, pos_count);
    }

    // 合并：负数倒序（取反后），然后正数
    size_t idx = 0;
    for (int i = (int)neg_count - 1; i >= 0; i--) {
        arr[idx++] = -negatives[i];
    }
    for (size_t i = 0; i < pos_count; i++) {
        arr[idx++] = positives[i];
    }

    free(negatives);
    free(positives);
}

/**
 * @brief 二进制基数排序（按位处理）
 */
void radix_sort_binary(int arr[], size_t n, int bits) {
    if (n <= 1) return;

    int *output = malloc(n * sizeof(int));
    int *temp = arr;
    int *swap;

    if (!output) return;

    for (int b = 0; b < bits; b++) {
        int count[2] = {0};

        // 统计
        for (size_t i = 0; i < n; i++) {
            int bit = (temp[i] >> b) & 1;
            count[bit]++;
        }

        // 累积
        count[1] += count[0];

        // 从后向前放置
        for (int i = (int)n - 1; i >= 0; i--) {
            int bit = (temp[i] >> b) & 1;
            output[--count[bit]] = temp[i];
        }

        // 交换输入输出
        swap = temp;
        temp = output;
        output = swap;
    }

    // 如果最终数据在output中，复制回arr
    if (temp != arr) {
        memcpy(arr, temp, n * sizeof(int));
    }

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
    int arr1[] = {170, 45, 75, 90, 2, 802, 24, 66};
    int arr2[] = {170, 45, 75, 90, 2, 802, 24, 66};
    int arr3[] = {-50, -10, 30, -5, 0, 20, -100, 80};
    size_t n = sizeof(arr1) / sizeof(arr1[0]);

    printf("=== 基数排序演示 ===\n\n");

    printf("LSD基数排序:\n");
    printf("原始: ");
    print_array(arr1, n);
    radix_sort_lsd(arr1, n);
    printf("排序: ");
    print_array(arr1, n);

    printf("\nMSD基数排序:\n");
    printf("原始: ");
    print_array(arr2, n);
    radix_sort_msd(arr2, n);
    printf("排序: ");
    print_array(arr2, n);

    printf("\n处理负数:\n");
    printf("原始: ");
    print_array(arr3, n);
    radix_sort_with_negatives(arr3, n);
    printf("排序: ");
    print_array(arr3, n);

    return 0;
}
