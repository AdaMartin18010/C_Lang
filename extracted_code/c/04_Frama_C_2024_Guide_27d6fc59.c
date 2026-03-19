/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\04_Frama_C_2024_Guide.md
 * Line: 679
 * Language: c
 * Block ID: 27d6fc59
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @ghost typedef struct { int count; } Counter;
*/

/*@
  @requires \valid(a + (0..n-1));
  @requires \valid_read(b + (0..n-1));
  @assigns a[0..n-1];
  @ensures \forall integer i; 0 <= i < n ==> a[i] == a[i] + b[i];
*/
void vector_add(int* a, const int* b, int n) {
    /*@ ghost Counter modified = {0}; */

    /*@
      @loop invariant 0 <= i <= n;
      @loop invariant modified.count == i;
      @loop assigns i, a[0..n-1], modified;
      @loop variant n - i;
    */
    for (int i = 0; i < n; i++) {
        a[i] += b[i];
        /*@ ghost modified.count++; */
    }

    /*@ assert modified.count == n; */
}
