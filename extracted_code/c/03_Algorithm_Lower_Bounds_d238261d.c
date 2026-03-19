/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\03_Algorithm_Lower_Bounds.md
 * Line: 556
 * Language: c
 * Block ID: d238261d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 证明线性搜索下界的实验框架
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 线性搜索 - 最坏情况O(n)
int linear_search(int arr[], int n, int target, int* comparisons) {
    *comparisons = 0;
    for (int i = 0; i < n; i++) {
        (*comparisons)++;
        if (arr[i] == target) return i;
    }
    return -1;
}

// 二分搜索 - O(log n)
int binary_search(int arr[], int n, int target, int* comparisons) {
    *comparisons = 0;
    int left = 0, right = n - 1;

    while (left <= right) {
        (*comparisons)++;
        int mid = left + (right - left) / 2;
        if (arr[mid] == target) return mid;
        if (arr[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

// 验证下界
void verify_bounds() {
    printf("搜索算法复杂度验证\n");
    printf("理论下界: 无序数组Ω(n), 有序数组Ω(log n)\n\n");

    for (int n = 10; n <= 1000000; n *= 10) {
        int* sorted = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) sorted[i] = i;

        int comps;
        // 搜索不存在的元素（最坏情况）
        binary_search(sorted, n, -1, &comps);
        printf("n=%d, 二分搜索比较次数=%d, log2(n)=%.1f\n",
               n, comps, log2(n));

        free(sorted);
    }
}
