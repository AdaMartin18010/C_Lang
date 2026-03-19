/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_WP_Tutorial.md
 * Line: 750
 * Language: c
 * Block ID: 45e5f254
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// wp_case1_binary_search.c

/*@
  @predicate is_sorted(int* a, integer n) =
    \forall integer i, j; 0 <= i <= j < n ==> a[i] <= a[j];
*/

/*@
  @requires n >= 0;
  @requires \valid_read(arr + (0..n-1));
  @requires is_sorted(arr, n);
  @assigns \nothing;

  // 功能规格
  @ensures -1 <= \result < n;
  @ensures \result >= 0 ==> arr[\result] == target;
  @ensures \result == -1 ==>
    \forall integer i; 0 <= i < n ==> arr[i] != target;
*/
int binary_search(const int* arr, int n, int target) {
    int low = 0;
    int high = n - 1;

    /*@
      @loop invariant 0 <= low;
      @loop invariant high < n;
      @loop invariant low <= high + 1;
      // 关键不变量: 如果目标存在,必在[low, high]范围内
      @loop invariant \forall integer i;
        (0 <= i < low || high < i < n) ==> arr[i] != target;
      @loop assigns low, high;
      @loop variant high - low;
    */
    while (low <= high) {
        // 防止溢出: mid = (low + high) / 2
        int mid = low + (high - low) / 2;

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
