/*
 * Auto-generated from: 12_Practice_Exercises\06_Performance_Optimization.md
 * Line: 217
 * Language: c
 * Block ID: 312e3759
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 二分查找 (分支多)
int binary_search_branchy(int *arr, int n, int key) {
    int low = 0, high = n - 1;
    while (low <= high) {
        int mid = (low + high) >> 1;
        if (arr[mid] == key) return mid;
        if (arr[mid] < key) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

// 无分支二分查找
int binary_search_branchless(int *arr, int n, int key) {
    int low = 0;
    int len = n;
    while (len > 1) {
        int half = len >> 1;
        len -= half;
        low = (arr[low + half - 1] < key) ? low + half : low;
    }
    return (arr[low] == key) ? low : -1;
}
