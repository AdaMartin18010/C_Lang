/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\09_Performance_Optimization.md
 * Line: 239
 * Language: c
 * Block ID: 44d6fc4d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优化1: 线性查找的哨兵优化
int linear_search(int *arr, int n, int key) {
    int last = arr[n - 1];
    arr[n - 1] = key;  // 哨兵

    int i = 0;
    while (arr[i] != key) i++;

    arr[n - 1] = last;
    return (i < n - 1 || last == key) ? i : -1;
}

// 优化2: 二分查找 (有序数据)
int binary_search(int *arr, int n, int key) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid] < key) lo = mid + 1;
        else if (arr[mid] > key) hi = mid - 1;
        else return mid;
    }
    return -1;
}

// 优化3: 插值查找 (均匀分布数据)
int interpolation_search(int *arr, int n, int key) {
    int lo = 0, hi = n - 1;
    while (lo <= hi && key >= arr[lo] && key <= arr[hi]) {
        int pos = lo + ((key - arr[lo]) * (hi - lo)) / (arr[hi] - arr[lo]);
        if (arr[pos] == key) return pos;
        if (arr[pos] < key) lo = pos + 1;
        else hi = pos - 1;
    }
    return -1;
}
