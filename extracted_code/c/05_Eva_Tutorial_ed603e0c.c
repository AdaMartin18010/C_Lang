/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Eva_Tutorial.md
 * Line: 467
 * Language: c
 * Block ID: ed603e0c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// tutorial4_binary_search.c

/*@
  @requires n >= 0;
  @requires \valid_read(arr + (0..n-1));
  @requires \forall integer i,j; 0 <= i <= j < n ==> arr[i] <= arr[j];
  @assigns \nothing;
  @ensures \result >= -1;
  @ensures \result >= 0 ==> arr[\result] == target;
*/
int binary_search(const int* arr, int n, int target) {
    int low = 0;
    int high = n - 1;

    // Eva推断:
    // 0 <= low <= high+1 <= n
    // 不变量: 目标如果存在, 必在[low, high]内
    while (low <= high) {
        int mid = low + (high - low) / 2;  // 防止溢出

        if (arr[mid] == target) {
            return mid;
        } else if (arr[mid] < target) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    return -1;
}
