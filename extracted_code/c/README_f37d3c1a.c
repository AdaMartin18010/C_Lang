/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Practical_Verification_Cases\README.md
 * Line: 177
 * Language: c
 * Block ID: f37d3c1a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @ requires \valid(a) && \valid(b);
  @ assigns *a, *b;
  @ ensures *a == \old(*b);
  @ ensures *b == \old(*a);
*/
void swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}
