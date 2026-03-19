/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_WP_Tutorial.md
 * Line: 814
 * Language: c
 * Block ID: 11dedf4b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// wp_case2_partition.c

/*@
  @predicate is_partitioned(int* a, integer n, integer pivot) =
    0 <= pivot < n ==>
    (\forall integer i; 0 <= i < pivot ==> a[i] <= a[pivot]) &&
    (\forall integer i; pivot < i < n ==> a[i] >= a[pivot]);
*/

/*@
  @requires n > 0;
  @requires \valid(a + (0..n-1));
  @assigns a[0..n-1];
  @ensures 0 <= \result < n;
  @ensures is_partitioned(a, n, \result);
*/
int partition(int* a, int n) {
    int pivot = a[n - 1];  // 选择最后一个元素为基准
    int i = 0;

    /*@
      @loop invariant 0 <= i <= j <= n;
      @loop invariant \forall integer k; 0 <= k < i ==> a[k] <= pivot;
      @loop invariant \forall integer k; i <= k < j ==> a[k] > pivot;
      @loop assigns i, j, a[0..n-1];
      @loop variant n - j;
    */
    for (int j = 0; j < n - 1; j++) {
        if (a[j] <= pivot) {
            // 交换 a[i] 和 a[j]
            int temp = a[i];
            a[i] = a[j];
            a[j] = temp;
            i++;
        }
    }

    // 将基准放到正确位置
    int temp = a[i];
    a[i] = a[n - 1];
    a[n - 1] = temp;

    return i;
}
