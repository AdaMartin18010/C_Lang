/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Practical_Verification_Cases\README.md
 * Line: 264
 * Language: c
 * Block ID: fb155324
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @ predicate sorted(int* a, integer n) =
  @   \forall integer i, j; 0 <= i <= j < n ==> a[i] <= a[j];
  @
  @ predicate permutation{L1,L2}(int* a, integer n) =
  @   \forall integer v; \numof(0, n-1, \lambda integer k; a[k] == v) ==
  @   \numof(0, n-1, \lambda integer k; \at(a[k], L1) == v);
*/

/*@
  @ requires \valid(a + (0..n-1));
  @ requires n >= 0;
  @ assigns a[0..n-1];
  @ ensures sorted(a, n);
  @ ensures permutation{Pre,Here}(a, n);
*/
void insertion_sort(int* a, size_t n) {
    /*@
      @ loop invariant 1 <= i <= n;
      @ loop invariant sorted(a, i);
      @ loop invariant permutation{Pre,Here}(a, n);
      @ loop assigns i, a[0..n-1];
      @ loop variant n - i;
    */
    for (size_t i = 1; i < n; i++) {
        int key = a[i];
        size_t j = i;

        /*@
          @ loop invariant 0 <= j <= i;
          @ loop invariant \forall integer k; j < k <= i ==> a[k] > key;
          @ loop assigns j, a[1..i];
          @ loop variant j;
        */
        while (j > 0 && a[j-1] > key) {
            a[j] = a[j-1];
            j--;
        }
        a[j] = key;
    }
}
