/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\09_Performance_Optimization.md
 * Line: 284
 * Language: c
 * Block ID: 1f084911
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优化1: 对小数组使用插入排序
void introsort(int *arr, int n);

// 优化2: 计数排序 (整数、范围小)
void counting_sort(uint8_t *arr, int n) {
    int count[256] = {0};
    for (int i = 0; i < n; i++) count[arr[i]]++;

    int k = 0;
    for (int i = 0; i < 256; i++) {
        while (count[i]-- > 0) arr[k++] = i;
    }
}

// 优化3: 基数排序 (整数)
void radix_sort(uint32_t *arr, int n);

// 优化4: 针对几乎有序数据的TimSort
// (现代标准库已实现)
