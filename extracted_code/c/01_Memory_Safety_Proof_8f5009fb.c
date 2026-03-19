/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Practical_Verification_Cases\01_Memory_Safety_Proof.md
 * Line: 80
 * Language: c
 * Block ID: 8f5009fb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  allocates \result;
  ensures \result != \null ==> \valid(\result);
  ensures \result != \null ==> \fresh(\result, sizeof(int) * n);
*/
int* create_array(size_t n);

/*@
  requires \valid(ptr);
  frees ptr;
*/
void destroy_array(int* ptr);
