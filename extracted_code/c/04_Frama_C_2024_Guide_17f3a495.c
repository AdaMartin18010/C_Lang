/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\04_Frama_C_2024_Guide.md
 * Line: 545
 * Language: c
 * Block ID: 17f3a495
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @requires n > 0;
  @requires \valid_read(src + (0..n-1));
  @assigns \nothing;
  @allocates \result;
  @ensures \result == \null || \valid(\result + (0..n-1));
  @ensures \result != \null ==>
    \forall integer i; 0 <= i < n ==> \result[i] == src[i];
  @ensures \result != \null ==> \initialized(\result + (0..n-1));
*/
int* clone_array(const int* src, size_t n);
