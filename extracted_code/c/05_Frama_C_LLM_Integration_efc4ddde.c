/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Frama_C_LLM_Integration.md
 * Line: 2170
 * Language: c
 * Block ID: efc4ddde
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @ requires n >= 0;
  @ ensures \\result == n * (n + 1) / 2;
  @ assigns \\nothing;
  @*/
int sum(int n);

/*@
  @ requires n >= 0;
  @ ensures \\result == n * (n + 1) / 2;
  @ assigns \\nothing;
  @*/
int sum(int n) {
    int s = 0;
    /*@
      @ loop invariant 1 <= i <= n + 1;
      @ loop invariant s == (i-1) * i / 2;
      @ loop assigns i, s;
      @ loop variant n - i + 1;
      @*/
    for (int i = 1; i <= n; i++) {
        s += i;
    }
    return s;
}
