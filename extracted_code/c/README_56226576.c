/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\README.md
 * Line: 252
 * Language: c
 * Block ID: 56226576
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用 ACSL (ANSI/ISO C Specification Language) 注释
/*@
  requires \valid(p) && \valid(q);
  requires \separated(p, q);
  assigns *p, *q;
  ensures *p == \old(*q);
  ensures *q == \old(*p);
*/
void swap(int *p, int *q) {
    int tmp = *p;
    *p = *q;
    *q = tmp;
}
