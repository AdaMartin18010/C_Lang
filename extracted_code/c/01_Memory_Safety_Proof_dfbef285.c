/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Practical_Verification_Cases\01_Memory_Safety_Proof.md
 * Line: 63
 * Language: c
 * Block ID: dfbef285
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  requires p != \null ==> \valid(p);
  behavior null_case:
    assumes p == \null;
    ensures \result == 0;
  behavior valid_case:
    assumes p != \null;
    ensures \result == *p + 1;
*/
int safe_increment(const int* p) {
    return (p == NULL) ? 0 : (*p + 1);
}
