/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\10_Formal_Verification.md
 * Line: 779
 * Language: c
 * Block ID: f09c0d9b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@ requires n >= 0;
    requires \valid_read(arr + (0..n-1));
    requires sorted: \forall integer i, j;
             0 <= i <= j < n ==> arr[i] <= arr[j];
    assigns \nothing;
    behavior found:
      assumes \exists integer i; 0 <= i < n && arr[i] == key;
      ensures 0 <= \result < n;
      ensures arr[\result] == key;
    behavior not_found:
      assumes \forall integer i; 0 <= i < n ==> arr[i] != key;
      ensures \result == -1;
    complete behaviors;
    disjoint behaviors;
*/
int binary_search(const int *arr, int n, int key) {
    int lo = 0;
    int hi = n - 1;

    /*@ loop invariant 0 <= lo <= n;
        loop invariant -1 <= hi < n;
        loop invariant not_found:
          \forall integer i; 0 <= i < n && arr[i] == key ==> lo <= i <= hi;
        loop assigns lo, hi;
        loop variant hi - lo;
    */
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;

        if (arr[mid] == key) {
            return mid;
        } else if (arr[mid] < key) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }

    return -1;
}
