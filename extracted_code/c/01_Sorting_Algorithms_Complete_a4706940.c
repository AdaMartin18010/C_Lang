/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\01_Sorting_Algorithms_Complete.md
 * Line: 810
 * Language: c
 * Block ID: a4706940
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 快速排序
 * 文件名: quick_sort.c
 * 编译: gcc -std=c23 -O2 -o quick_sort quick_sort.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static inline void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * @brief Lomuto分区方案
 * @return pivot的最终位置
 */
size_t lomuto_partition(int arr[], size_t low, size_t high) {
    int pivot = arr[high];
    size_t i = low;

    for (size_t j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            swap(&arr[i], &arr[j]);
            i++;
        }
    }

    swap(&arr[i], &arr[high]);
    return i;
}

/**
 * @brief Hoare分区方案 - 更高效
 * @return 分区边界
 */
size_t hoare_partition(int arr[], size_t low, size_t high) {
    int pivot = arr[low + (high - low) / 2];
    size_t i = low;
    size_t j = high;

    while (true) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;

        if (i >= j) return j;

        swap(&arr[i], &arr[j]);
        i++;
        j--;
    }
}

/**
 * @brief 三数取中 - 选择更好的pivot
 */
int median_of_three(int arr[], size_t low, size_t high) {
    size_t mid = low + (high - low) / 2;

    if (arr[low] > arr[mid]) swap(&arr[low], &arr[mid]);
    if (arr[low] > arr[high]) swap(&arr[low], &arr[high]);
    if (arr[mid] > arr[high]) swap(&arr[mid], &arr[high]);

    // arr[mid]是中间值
    swap(&arr[mid], &arr[high - 1]);
    return arr[high - 1];
}

/**
 * @brief 基础快速排序 - 使用Lomuto分区
 */
void quick_sort_basic(int arr[], size_t low, size_t high) {
    if (low < high) {
        size_t pi = lomuto_partition(arr, low, high);

        if (pi > 0) {
            quick_sort_basic(arr, low, pi - 1);
        }
        quick_sort_basic(arr, pi + 1, high);
    }
}

/**
 * @brief 优化快速排序
 * - 使用Hoare分区
 * - 小数组切换为插入排序
 * - 三数取中优化
 */
void quick_sort_optimized_helper(int arr[], size_t low, size_t high);

#define INSERTION_THRESHOLD 16

void quick_sort_optimized(int arr[], size_t n) {
    // 先对数组进行随机打乱，避免最坏情况
    srand((unsigned)time(NULL));
    for (size_t i = n - 1; i > 0; i--) {
        size_t j = rand() % (i + 1);
        swap(&arr[i], &arr[j]);
    }

    quick_sort_optimized_helper(arr, 0, n - 1);
}

void quick_sort_optimized_helper(int arr[], size_t low, size_t high) {
    // 小数组使用插入排序
    if (high - low + 1 <= INSERTION_THRESHOLD) {
        for (size_t i = low + 1; i <= high; i++) {
            int key = arr[i];
            int j = (int)i - 1;
            while (j >= (int)low && arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
        return;
    }

    if (low < high) {
        size_t pi = hoare_partition(arr, low, high);
        quick_sort_optimized_helper(arr, low, pi);
        quick_sort_optimized_helper(arr, pi + 1, high);
    }
}

/**
 * @brief 三路快速排序 - 处理大量重复元素
 */
void quick_sort_3way(int arr[], size_t low, size_t high) {
    if (high <= low) return;

    size_t lt = low;      // arr[low..lt-1] < pivot
    size_t gt = high;     // arr[gt+1..high] > pivot
    int pivot = arr[low];
    size_t i = low + 1;

    while (i <= gt) {
        if (arr[i] < pivot) {
            swap(&arr[lt], &arr[i]);
            lt++;
            i++;
        } else if (arr[i] > pivot) {
            swap(&arr[i], &arr[gt]);
            gt--;
        } else {
            i++;
        }
    }

    // arr[lt..gt] == pivot
    if (lt > 0) {
        quick_sort_3way(arr, low, lt - 1);
    }
    quick_sort_3way(arr, gt + 1, high);
}

/**
 * @brief 尾递归优化版本
 */
void quick_sort_tail_recursive(int arr[], size_t low, size_t high) {
    while (low < high) {
        size_t pi = hoare_partition(arr, low, high);

        // 先排序较小的一半，减少递归深度
        if (pi - low < high - pi) {
            quick_sort_tail_recursive(arr, low, pi);
            low = pi + 1;
        } else {
            quick_sort_tail_recursive(arr, pi + 1, high);
            high = pi;
        }
    }
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
    int arr1[] = {64, 34, 25, 12, 22, 11, 90, 5, 77, 30};
    int arr2[] = {64, 34, 25, 12, 22, 11, 90, 5, 77, 30};
    int arr3[] = {2, 2, 1, 3, 2, 1, 3, 2, 1, 3};  // 大量重复
    size_t n = sizeof(arr1) / sizeof(arr1[0]);

    printf("=== 快速排序演示 ===\n");
    printf("原始数组: ");
    print_array(arr1, n);

    quick_sort_basic(arr1, 0, n - 1);
    printf("基础快排: ");
    print_array(arr1, n);

    quick_sort_optimized(arr2, n);
    printf("优化快排: ");
    print_array(arr2, n);

    printf("\n三路快排（重复元素）:\n");
    printf("原始: ");
    print_array(arr3, n);
    quick_sort_3way(arr3, 0, n - 1);
    printf("排序: ");
    print_array(arr3, n);

    return 0;
}
