/*
 * Auto-generated from: 12_Practice_Exercises\01_Basic_Exercises.md
 * Line: 2295
 * Language: c
 * Block ID: b610eefd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int lower_bound(int arr[], int n, int target) {
    int left = 0, right = n;  // 注意right=n
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;  // 返回第一个>=target的位置
}
