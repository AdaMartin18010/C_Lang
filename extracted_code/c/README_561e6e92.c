/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Practical_Verification_Cases\README.md
 * Line: 120
 * Language: c
 * Block ID: 561e6e92
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @ requires n > 0;
  @ requires \valid(elem);
  @ allocates \result;
  @ assigns \result \from elem, n;
  @ ensures \result != \null ==> \fresh(\result, sizeof(int) * n);
  @ ensures \result != \null ==> \valid(((int*)\result) + (0..n-1));
  @ behavior success:
  @   assumes \result != \null;
  @   ensures \forall integer i; 0 <= i < n ==> ((int*)\result)[i] == *elem;
  @ behavior failure:
  @   assumes \result == \null;
  @   allocates \nothing;
*/
int* create_filled_array(size_t n, const int* elem) {
    int* arr = (int*)malloc(sizeof(int) * n);
    if (arr == NULL) {
        return NULL;
    }

    /*@
      @ loop invariant 0 <= i <= n;
      @ loop invariant \valid(arr + (0..n-1));
      @ loop assigns i, arr[0..n-1];
      @ loop variant n - i;
    */
    for (size_t i = 0; i < n; i++) {
        arr[i] = *elem;
    }

    return arr;
}
