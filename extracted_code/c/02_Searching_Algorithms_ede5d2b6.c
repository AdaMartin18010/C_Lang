/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\02_Searching_Algorithms.md
 * Line: 1474
 * Language: c
 * Block ID: ede5d2b6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 二分查找最接近的值
 */
int binary_search_nearest(const int arr[], size_t n, int target) {
    if (n == 0) return -1;

    size_t left = 0, right = n - 1;

    while (left < right) {
        size_t mid = left + (right - left) / 2;

        if (arr[mid] == target) return (int)mid;
        if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    // left == right，检查哪个更接近
    if (left > 0 && abs(arr[left - 1] - target) < abs(arr[left] - target)) {
        return (int)left - 1;
    }
    return (int)left;
}
