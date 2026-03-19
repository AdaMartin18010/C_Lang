/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_WP_Tutorial.md
 * Line: 493
 * Language: c
 * Block ID: 33a51d51
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// wp_tutorial1.c

/*@
  @requires \valid(p);
  @requires *p >= 0;
  @assigns *p;
  @ensures *p == \old(*p) + 1;
  @ensures *p > 0;
*/
void increment(int* p) {
    *p = *p + 1;
}

/*@
  @requires n >= 0;
  @requires \valid_read(arr + (0..n-1));
  @assigns \nothing;
  @ensures \result >= 0;
  @ensures \forall integer i; 0 <= i < n ==> \result >= arr[i];
  @ensures \exists integer i; 0 <= i < n && \result == arr[i];
*/
int array_max(const int* arr, int n) {
    if (n == 0) return 0;

    int max = arr[0];
    /*@
      @loop invariant 1 <= i <= n;
      @loop invariant \forall integer j; 0 <= j < i ==> max >= arr[j];
      @loop invariant \exists integer j; 0 <= j < i && max == arr[j];
      @loop assigns i, max;
      @loop variant n - i;
    */
    for (int i = 1; i < n; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}
