/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Practical_Verification_Cases\README.md
 * Line: 97
 * Language: c
 * Block ID: 293d043e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @ requires p != \null ==> \valid(p);
  @ assigns \nothing;
  @ behavior null_case:
  @   assumes p == \null;
  @   ensures \result == 0;
  @ behavior valid_case:
  @   assumes p != \null;
  @   ensures \result == *p;
  @ complete behaviors;
  @ disjoint behaviors;
*/
int safe_dereference(int* p) {
    if (p == NULL) {
        return 0;
    }
    return *p;
}
