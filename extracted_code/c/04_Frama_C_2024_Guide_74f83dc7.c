/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\04_Frama_C_2024_Guide.md
 * Line: 650
 * Language: c
 * Block ID: 74f83dc7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @logic integer sum_array(int* a, integer n) reads a[0..n-1];

  @axiom sum_empty:
    \forall int* a; sum_array(a, 0) == 0;

  @axiom sum_step:
    \forall int* a, integer n; n > 0 ==>
      sum_array(a, n) == sum_array(a, n-1) + a[n-1];

  @lemma sum_nonneg:
    \forall int* a, integer n;
      n >= 0 ==> (\forall integer i; 0 <= i < n ==> a[i] >= 0) ==>
      sum_array(a, n) >= 0;
*/

/*@
  @requires n >= 0 && \valid_read(arr + (0..n-1));
  @requires \forall integer i; 0 <= i < n ==> arr[i] >= 0;
  @assigns \nothing;
  @ensures \result == sum_array(arr, n);
  @ensures \result >= 0;
*/
int sum_positive(const int* arr, int n);
