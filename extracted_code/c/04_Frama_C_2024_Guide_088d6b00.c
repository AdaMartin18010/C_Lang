/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\04_Frama_C_2024_Guide.md
 * Line: 502
 * Language: c
 * Block ID: 088d6b00
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @requires n >= 0;
  @requires \valid_read(arr + (0..n-1));
  @assigns \nothing;
  @ensures \result >= 0;
  @ensures \forall integer i; 0 <= i < n ==> \result >= arr[i];
  @ensures \exists integer i; 0 <= i < n && \result == arr[i];
*/
int array_max(const int* arr, int n);
