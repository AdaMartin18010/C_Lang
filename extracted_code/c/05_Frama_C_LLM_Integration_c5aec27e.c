/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Frama_C_LLM_Integration.md
 * Line: 205
 * Language: c
 * Block ID: c5aec27e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 原始代码 */
int max(int a, int b) {
    return (a > b) ? a : b;
}

/* LLM生成的ACSL规范 */
/*@
  @ requires \valid(&a) && \valid(&b);
  @ ensures \result == (a > b ? a : b);
  @ ensures \result >= a && \result >= b;
  @ ensures \result == a || \result == b;
  @ assigns \nothing;
  @*/
int max(int a, int b) {
    return (a > b) ? a : b;
}
