/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Practical_Verification_Cases\README.md
 * Line: 45
 * Language: c
 * Block ID: e9b8035b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @ requires \valid(p) && n >= 0;
  @ requires \valid_read(src + (0..n-1));
  @ assigns *(p+(0..n-1));
  @ ensures \forall integer i; 0 <= i < n ==> p[i] == src[i];
  @ ensures \result == p;
*/
void* my_memcpy(void* p, const void* src, size_t n);
