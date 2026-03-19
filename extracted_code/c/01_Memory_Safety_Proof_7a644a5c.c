/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Practical_Verification_Cases\01_Memory_Safety_Proof.md
 * Line: 40
 * Language: c
 * Block ID: 7a644a5c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  requires \valid(arr + (0..n-1));
  requires n > 0;
  assigns \nothing;
*/
int array_max(const int* arr, size_t n) {
    int max = arr[0];
    /*@
      loop invariant 0 <= i <= n;
      loop invariant \forall integer j; 0 <= j < i ==> max >= arr[j];
      loop assigns i, max;
      loop variant n - i;
    */
    for (size_t i = 1; i < n; i++) {
        if (arr[i] > max) max = arr[i];
    }
    return max;
}
