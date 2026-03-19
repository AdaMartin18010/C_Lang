/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\02_P_vs_NP.md
 * Line: 335
 * Language: c
 * Block ID: 512cdd48
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 归并排序 - O(n log n)
void merge_sort(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(arr, left, mid);
        merge_sort(arr, mid + 1, right);
        merge(arr, left, mid, right);  // 线性时间合并
    }
}
