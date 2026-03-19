/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_WP_Tutorial.md
 * Line: 729
 * Language: c
 * Block ID: c726268b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 分离逻辑证明
/*@
  @requires \valid(p);
  @requires \valid(q);
  @requires \separated(p, q);
  @assigns *p, *q;
  @ensures *p == \old(*p) + 1;
  @ensures *q == \old(*q);
*/
void increment_p_only(int* p, int* q) {
    (*p)++;
}
