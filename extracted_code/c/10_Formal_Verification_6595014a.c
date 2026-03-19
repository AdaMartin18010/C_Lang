/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\10_Formal_Verification.md
 * Line: 274
 * Language: c
 * Block ID: 6595014a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 示例: 数组求和
int array_sum(int *arr, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

// 不变式发现过程:
// 1. 观察: sum在每次迭代后包含什么?
//    - i=0后: sum = arr[0]
//    - i=1后: sum = arr[0] + arr[1]
//    - i=k后: sum = arr[0] + ... + arr[k]
//
// 2. 不变式:
//    sum = Σ(arr[j], j=0 to i-1)
//    0 <= i <= n
//
// 3. 终止时 i=n, 所以 sum = Σ(arr[j], j=0 to n-1)

// 示例: 二分查找
int binary_search(int *arr, int n, int key) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid] == key) return mid;
        if (arr[mid] < key) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

// 不变式: 如果key在数组中，key在arr[lo..hi]中
// 即: ∃j. lo <= j <= hi && arr[j] == key
//     || key不在数组中
