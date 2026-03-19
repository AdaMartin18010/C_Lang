/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Practical_Verification_Cases\README.md
 * Line: 73
 * Language: c
 * Block ID: cc989994
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @ requires \valid(a + (0..n-1));
  @ requires n >= 0;
  @ assigns \nothing;
  @ ensures \result == \sum(0, n-1, \lambda integer k; a[k]);
*/
int array_sum(const int* a, size_t n) {
    int sum = 0;
    /*@
      @ loop invariant 0 <= i <= n;
      @ loop invariant sum == \sum(0, i-1, \lambda integer k; a[k]);
      @ loop assigns i, sum;
      @ loop variant n - i;
    */
    for (size_t i = 0; i < n; i++) {
        sum += a[i];
    }
    return sum;
}
