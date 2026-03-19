/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\05_Computational_Complexity\01_Complexity_Classes.md
 * Line: 104
 * Language: c
 * Block ID: e81de3d0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 归并排序 - O(n log n) ∈ P
void merge_sort(int arr[], int n) {
    if (n <= 1) return;

    int mid = n / 2;
    merge_sort(arr, mid);
    merge_sort(arr + mid, n - mid);
    merge(arr, mid, n - mid);  // O(n)合并
}
// 总时间: O(n log n) - 多项式时间
