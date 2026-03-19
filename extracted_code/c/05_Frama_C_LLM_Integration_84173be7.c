/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Frama_C_LLM_Integration.md
 * Line: 359
 * Language: c
 * Block ID: 84173be7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @ predicate sorted{L}(int *a, integer n) =
  @   \forall integer i, j; 0 <= i <= j < n ==> a[i] <= a[j];
  @
  @ predicate in_array{L}(int *a, integer n, int val) =
  @   \exists integer i; 0 <= i < n && a[i] == val;
  @*/

/*@
  @ requires n >= 0;
  @ requires \valid(arr+(0..n-1));
  @ requires sorted(arr, n);
  @
  @ behavior found:
  @   assumes in_array(arr, n, target);
  @   ensures 0 <= \result < n;
  @   ensures arr[\result] == target;
  @
  @ behavior not_found:
  @   assumes !in_array(arr, n, target);
  @   ensures \result == -1;
  @
  @ assigns \nothing;
  @*/
int binary_search(int *arr, int n, int target);
