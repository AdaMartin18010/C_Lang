/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\01_Sorting_Algorithms_Complete.md
 * Line: 1406
 * Language: c
 * Block ID: 2d791bec
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 堆排序
 * 文件名: heap_sort.c
 * 编译: gcc -std=c23 -O2 -o heap_sort heap_sort.c
 */

#include <stdio.h>
#include <stdbool.h>

static inline void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * @brief 下沉操作 - 维护堆性质
 * @param arr 数组
 * @param n 堆大小
 * @param i 需要下沉的节点
 */
void heapify_down(int arr[], size_t n, size_t i) {
    while (true) {
        size_t largest = i;
        size_t left = 2 * i + 1;
        size_t right = 2 * i + 2;

        if (left < n && arr[left] > arr[largest]) {
            largest = left;
        }
        if (right < n && arr[right] > arr[largest]) {
            largest = right;
        }

        if (largest == i) break;

        swap(&arr[i], &arr[largest]);
        i = largest;
    }
}

/**
 * @brief 上浮操作
 */
void heapify_up(int arr[], size_t i) {
    while (i > 0) {
        size_t parent = (i - 1) / 2;
        if (arr[i] <= arr[parent]) break;
        swap(&arr[i], &arr[parent]);
        i = parent;
    }
}

/**
 * @brief 建堆 - Floyd算法，O(n)
 */
void build_heap(int arr[], size_t n) {
    // 从最后一个非叶子节点开始下沉
    for (int i = (int)(n / 2) - 1; i >= 0; i--) {
        heapify_down(arr, n, (size_t)i);
    }
}

/**
 * @brief 基础堆排序
 */
void heap_sort_basic(int arr[], size_t n) {
    if (n <= 1) return;

    // 建堆
    build_heap(arr, n);

    // 排序
    for (size_t i = n - 1; i > 0; i--) {
        swap(&arr[0], &arr[i]);
        heapify_down(arr, i, 0);
    }
}

/**
 * @brief 优化的堆排序 - 减少比较次数
 */
void heap_sort_optimized(int arr[], size_t n) {
    if (n <= 1) return;

    build_heap(arr, n);

    for (size_t i = n - 1; i > 0; i--) {
        // 保存堆顶元素
        int max_val = arr[0];

        // 找到最终位置
        size_t parent = 0;
        size_t child = 2 * parent + 1;

        // 下沉但不交换
        while (child < i) {
            if (child + 1 < i && arr[child + 1] > arr[child]) {
                child++;
            }
            if (arr[child] <= max_val) break;
            arr[parent] = arr[child];
            parent = child;
            child = 2 * parent + 1;
        }

        arr[parent] = max_val;
        arr[i] = max_val;  // 实际上已经交换了
    }
}

/**
 * @brief 最小堆排序 - 升序排列（堆顶是最小值）
 */
void heapify_min(int arr[], size_t n, size_t i) {
    while (true) {
        size_t smallest = i;
        size_t left = 2 * i + 1;
        size_t right = 2 * i + 2;

        if (left < n && arr[left] < arr[smallest]) {
            smallest = left;
        }
        if (right < n && arr[right] < arr[smallest]) {
            smallest = right;
        }

        if (smallest == i) break;
        swap(&arr[i], &arr[smallest]);
        i = smallest;
    }
}

void build_min_heap(int arr[], size_t n) {
    for (int i = (int)(n / 2) - 1; i >= 0; i--) {
        heapify_min(arr, n, (size_t)i);
    }
}

/**
 * @brief 双堆排序 - 同时维护最大堆和最小堆
 */
void heap_sort_dual(int arr[], size_t n) {
    if (n <= 1) return;

    size_t left = 0;
    size_t right = n - 1;

    while (left < right) {
        // 构建最大堆用于左边
        build_heap(arr + left, right - left + 1);
        swap(&arr[left], &arr[right]);
        right--;

        if (left >= right) break;

        // 构建最小堆用于右边
        build_min_heap(arr + left, right - left + 1);
        swap(&arr[left], &arr[right]);
        left++;
    }
}

/**
 * @brief 平滑排序(Smooth Sort) - 堆排序的改进
 * @note 优先使用已知的升序序列，达到O(n)最好情况
 */
static const size_t leonardo_numbers[] = {
    1, 1, 3, 5, 9, 15, 25, 41, 67, 109, 177, 287, 465, 753, 1219,
    1973, 3193, 5167, 8361, 13529, 21891, 35421, 57313, 92735,
    150049, 242785, 392835, 635621, 1028457, 1664079
};

void smooth_sort(int arr[], size_t n) {
    if (n <= 1) return;

    // Leonardo树的数量和根节点
    size_t roots[64] = {0};
    int num_trees = 0;
    size_t size = 0;

    // 构建Leonardo堆
    for (size_t i = 0; i < n; i++) {
        if (num_trees >= 2 &&
            leonardo_numbers[num_trees - 2] == leonardo_numbers[num_trees - 1]) {
            // 合并两个相同大小的树
            roots[num_trees - 2] = i;
            num_trees--;
        } else {
            roots[num_trees++] = i;
        }
        size++;

        // 恢复堆性质
        size_t k = num_trees - 1;
        size_t j = i;
        while (k > 0) {
            size_t left_child = j - leonardo_numbers[k] + leonardo_numbers[k - 1];
            size_t right_child = j - 1;

            size_t max_child = left_child;
            if (k > 1 && arr[right_child] > arr[left_child]) {
                max_child = right_child;
            }

            if (arr[max_child] <= arr[j]) break;
            swap(&arr[j], &arr[max_child]);
            j = max_child;
            k = (k > 1 && max_child == right_child) ? k - 2 : k - 1;
        }
    }

    // 逐个提取最大值
    for (size_t i = n - 1; i > 0; i--) {
        if (num_trees > 1) {
            // 分割树
            size_t k = num_trees - 1;
            size_t right_root = roots[k];
            size_t left_root = roots[k] - leonardo_numbers[k - 1];

            num_trees--;
            roots[num_trees - 1] = right_root;

            // 恢复堆性质
            size_t j = left_root;
            k = num_trees - 1;
            while (k > 0) {
                size_t lc = j - leonardo_numbers[k] + leonardo_numbers[k - 1];
                size_t rc = j - 1;

                size_t mc = lc;
                if (k > 1 && arr[rc] > arr[lc]) mc = rc;

                if (arr[mc] <= arr[j]) break;
                swap(&arr[j], &arr[mc]);
                j = mc;
                k = (k > 1 && mc == rc) ? k - 2 : k - 1;
            }
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
    int arr3[] = {64, 34, 25, 12, 22, 11, 90, 5, 77, 30};
    size_t n = sizeof(arr1) / sizeof(arr1[0]);

    printf("=== 堆排序演示 ===\n");
    printf("原始数组: ");
    print_array(arr1, n);

    heap_sort_basic(arr1, n);
    printf("基础堆排序: ");
    print_array(arr1, n);

    heap_sort_optimized(arr2, n);
    printf("优化堆排序: ");
    print_array(arr2, n);

    smooth_sort(arr3, n);
    printf("平滑排序: ");
    print_array(arr3, n);

    return 0;
}
