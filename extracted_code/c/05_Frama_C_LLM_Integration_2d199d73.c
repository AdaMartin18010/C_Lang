/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Frama_C_LLM_Integration.md
 * Line: 2275
 * Language: c
 * Block ID: 2d199d73
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @ loop invariant 0 <= len;
  @ loop invariant \\forall integer i; 0 <= i < len ==> s[i] != '\\0';
  @ loop invariant \\valid(s+len);
  @ loop assigns len;
  @ loop variant INT_MAX - len; // 假设字符串有限长
  @*/
