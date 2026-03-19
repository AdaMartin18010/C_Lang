/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\04_Frama_C_2024_Guide.md
 * Line: 586
 * Language: c
 * Block ID: bd523f74
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @requires n >= 0 && m >= 0;
  @requires \valid_read(a + (0..n-1));
  @requires \valid_read(b + (0..m-1));
  @requires \valid(c + (0..n+m-1));
  @requires \separated(c + (0..n+m-1), a + (0..n-1), b + (0..m-1));
  @assigns c[0..n+m-1];
  @ensures \forall integer i; 0 <= i < n ==> c[i] == a[i];
  @ensures \forall integer i; 0 <= i < m ==> c[n+i] == b[i];
*/
void array_concat(const int* a, int n, const int* b, int m, int* c) {
    /*@
      @loop invariant 0 <= i <= n;
      @loop invariant \forall integer j; 0 <= j < i ==> c[j] == a[j];
      @loop assigns i, c[0..n-1];
      @loop variant n - i;
    */
    for (int i = 0; i < n; i++) {
        c[i] = a[i];
    }

    /*@
      @loop invariant 0 <= j <= m;
      @loop invariant \forall integer k; 0 <= k < n ==> c[k] == a[k];
      @loop invariant \forall integer k; 0 <= k < j ==> c[n+k] == b[k];
      @loop assigns j, c[n..n+m-1];
      @loop variant m - j;
    */
    for (int j = 0; j < m; j++) {
        c[n + j] = b[j];
    }
}
