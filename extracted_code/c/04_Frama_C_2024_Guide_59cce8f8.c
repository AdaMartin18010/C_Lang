/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\04_Frama_C_2024_Guide.md
 * Line: 563
 * Language: c
 * Block ID: 59cce8f8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @requires n >= 0 && \valid_read(arr + (0..n-1));
  @assigns \nothing;
  @ensures \result == \sum(0,n-1,\lambda integer k; arr[k]);
*/
int array_sum(const int* arr, int n) {
    int sum = 0;
    /*@
      @loop invariant 0 <= i <= n;
      @loop invariant sum == \sum(0,i-1,\lambda integer k; arr[k]);
      @loop assigns i, sum;
      @loop variant n - i;
    */
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}
