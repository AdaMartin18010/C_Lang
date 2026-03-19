/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\04_Frama_C_2024_Guide.md
 * Line: 458
 * Language: c
 * Block ID: baba01b1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @requires \valid(p) && \valid(q);
  @requires *p >= 0 && *q >= 0;
  @assigns \nothing;
  @ensures \result == *p + *q;
  @ensures \result >= *p && \result >= *q;
*/
int add(const int* p, const int* q) {
    return *p + *q;
}
