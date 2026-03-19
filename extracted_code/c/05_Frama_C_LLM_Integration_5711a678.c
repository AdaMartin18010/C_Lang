/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Frama_C_LLM_Integration.md
 * Line: 2251
 * Language: c
 * Block ID: 5711a678
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @ loop invariant 0 <= i <= n;
  @ loop invariant c == \\numof(0, i-1, \\lambda integer k; arr[k] == x);
  @ loop assigns i, c;
  @ loop variant n - i;
  @*/
