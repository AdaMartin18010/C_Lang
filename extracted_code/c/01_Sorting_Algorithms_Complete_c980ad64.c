/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\01_Sorting_Algorithms_Complete.md
 * Line: 1080
 * Language: c
 * Block ID: c980ad64
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 归并排序
 * 文件名: merge_sort.c
 * 编译: gcc -std=c23 -O2 -o merge_sort merge_sort.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 合并两个有序数组
 * @param arr 原数组
 * @param left 左半部分起始
 * @param mid 中间位置
 * @param right 右半部分结束
 * @param temp 临时数组
 */
void merge(int arr[], size_t left, size_t mid, size_t right, int temp[]) {
    size_t i = left;      // 左半部分索引
    size_t j = mid + 1;   // 右半部分索引
    size_t k = 0;         // 临时数组索引

    // 合并两个有序数组
    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }

    // 复制剩余元素
    while (i <= mid) {
        temp[k++] = arr[i++];
    }
    while (j <= right) {
        temp[k++] = arr[j++];
    }

    // 复制回原始数组
    for (i = 0; i < k; i++) {
        arr[left + i] = temp[i];
    }
}

/**
 * @brief 自顶向下归并排序
 */
void merge_sort_topdown_helper(int arr[], size_t left, size_t right, int temp[]) {
    if (left < right) {
        size_t mid = left + (right - left) / 2;

        merge_sort_topdown_helper(arr, left, mid, temp);
        merge_sort_topdown_helper(arr, mid + 1, right, temp);
        merge(arr, left, mid, right, temp);
    }
}

void merge_sort_topdown(int arr[], size_t n) {
    if (n <= 1) return;

    int *temp = malloc(n * sizeof(int));
    if (!temp) return;

    merge_sort_topdown_helper(arr, 0, n - 1, temp);
    free(temp);
}

/**
 * @brief 自底向上归并排序 - 非递归
 */
void merge_sort_bottomup(int arr[], size_t n) {
    if (n <= 1) return;

    int *temp = malloc(n * sizeof(int));
    if (!temp) return;

    // 从大小为1开始，逐步倍增
    for (size_t width = 1; width < n; width *= 2) {
        for (size_t i = 0; i < n; i += 2 * width) {
            size_t left = i;
            size_t mid = (i + width < n) ? i + width - 1 : n - 1;
            size_t right = (i + 2 * width < n) ? i + 2 * width - 1 : n - 1;

            if (mid < right) {
                merge(arr, left, mid, right, temp);
            }
        }
    }

    free(temp);
}

/**
 * @brief 原地归并排序（使用旋转操作，非稳定）
 */
void rotate(int arr[], size_t l, size_t m, size_t r) {
    // 旋转数组[l..m]和[m+1..r]
    size_t i = l, j = m + 1;
    while (i < j && j <= r) {
        // 找到第一个arr[i] > arr[j]的位置
        while (i < j && arr[i] <= arr[j]) i++;

        // 找到连续的大于arr[j]的区间
        size_t k = j;
        while (k <= r && arr[k] < arr[i]) k++;

        // 旋转[i..j-1]和[j..k-1]
        if (i < j && j < k) {
            // 三次反转实现旋转
            size_t a = i, b = j - 1;
            while (a < b) {
                int t = arr[a]; arr[a] = arr[b]; arr[b] = t;
                a++; b--;
            }
            a = j; b = k - 1;
            while (a < b) {
                int t = arr[a]; arr[a] = arr[b]; arr[b] = t;
                a++; b--;
            }
            a = i; b = k - 1;
            while (a < b) {
                int t = arr[a]; arr[a] = arr[b]; arr[b] = t;
                a++; b--;
            }
        }

        i += (k - j);
        j = k;
    }
}

/**
 * @brief 自然归并排序 - 利用已有序的连续序列
 */
void merge_sort_natural(int arr[], size_t n) {
    if (n <= 1) return;

    int *temp = malloc(n * sizeof(int));
    if (!temp) return;

    while (true) {
        // 找自然有序序列并合并
        size_t i = 0;
        bool merged = false;

        while (i < n) {
            // 找第一个有序序列的结束
            size_t start1 = i;
            while (i + 1 < n && arr[i] <= arr[i + 1]) i++;
            size_t end1 = i;

            if (i + 1 >= n) break;

            // 找第二个有序序列
            size_t start2 = i + 1;
            i++;
            while (i + 1 < n && arr[i] <= arr[i + 1]) i++;
            size_t end2 = i;

            // 合并
            merge(arr, start1, end1, end2, temp);
            merged = true;
            i = end2 + 1;
        }

        if (!merged) break;
    }

    free(temp);
}

/**
 * @brief 多路归并排序 - 适合外部排序
 */
void merge_sort_multiway(int arr[], size_t n, int k) {
    if (n <= 1 || k < 2) return;

    int *temp = malloc(n * sizeof(int));
    if (!temp) return;

    // k路归并
    size_t width = 1;
    while (width < n) {
        size_t next_width = width * k;

        for (size_t i = 0; i < n; i += next_width) {
            // 收集k个有序序列的信息
            size_t starts[16];  // 最多16路
            size_t ends[16];
            int num_runs = 0;

            for (int j = 0; j < k && i + j * width < n; j++) {
                starts[j] = i + j * width;
                ends[j] = (i + (j + 1) * width < n) ?
                          i + (j + 1) * width - 1 : n - 1;
                if (starts[j] <= ends[j]) {
                    num_runs++;
                }
            }

            // k路合并
            size_t idx = 0;
            size_t dest = i;
            while (num_runs > 0) {
                // 找最小值
                int min_val = arr[starts[0]];
                int min_idx = 0;
                for (int j = 1; j < k; j++) {
                    if (starts[j] <= ends[j] && arr[starts[j]] < min_val) {
                        min_val = arr[starts[j]];
                        min_idx = j;
                    }
                }

                temp[idx++] = min_val;
                starts[min_idx]++;
                if (starts[min_idx] > ends[min_idx]) {
                    num_runs--;
                }
            }

            // 复制回数组
            for (size_t t = 0; t < idx; t++) {
                arr[dest + t] = temp[t];
            }
        }

        width = next_width;
    }

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
    int arr1[] = {64, 34, 25, 12, 22, 11, 90, 5};
    int arr2[] = {64, 34, 25, 12, 22, 11, 90, 5};
    int arr3[] = {64, 34, 25, 12, 22, 11, 90, 5};
    size_t n = sizeof(arr1) / sizeof(arr1[0]);

    printf("=== 归并排序演示 ===\n");
    printf("原始数组: ");
    print_array(arr1, n);

    merge_sort_topdown(arr1, n);
    printf("自顶向下: ");
    print_array(arr1, n);

    merge_sort_bottomup(arr2, n);
    printf("自底向上: ");
    print_array(arr2, n);

    merge_sort_natural(arr3, n);
    printf("自然归并: ");
    print_array(arr3, n);

    return 0;
}
