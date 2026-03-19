/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\02_Randomized_Algorithms.md
 * Line: 85
 * Language: c
 * Block ID: 0b23e39a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 拉斯维加斯算法示例：快速排序随机化
#include <stdlib.h>
#include <time.h>

// 随机选择枢轴
int randomized_partition(int *arr, int low, int high) {
    // 随机选择枢轴位置
    int random_idx = low + rand() % (high - low + 1);

    // 交换到末尾
    int temp = arr[random_idx];
    arr[random_idx] = arr[high];
    arr[high] = temp;

    // 标准分区
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }

    temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;

    return i + 1;
}

void randomized_quicksort(int *arr, int low, int high) {
    if (low < high) {
        int pi = randomized_partition(arr, low, high);
        randomized_quicksort(arr, low, pi - 1);
        randomized_quicksort(arr, pi + 1, high);
    }
}

// 期望时间复杂度：O(n log n)，总是正确
