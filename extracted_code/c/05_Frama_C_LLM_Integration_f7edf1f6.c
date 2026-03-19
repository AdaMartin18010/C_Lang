/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Frama_C_LLM_Integration.md
 * Line: 2212
 * Language: c
 * Block ID: f7edf1f6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @ requires n >= 0;
  @ requires \\valid(arr+(0..n-1));
  @ behavior found:
  @   assumes \\exists integer i; 0 <= i < n && arr[i] == x;
  @   ensures 0 <= \\result < n;
  @   ensures arr[\\result] == x;
  @ behavior not_found:
  @   assumes \\forall integer i; 0 <= i < n ==> arr[i] != x;
  @   ensures \\result == -1;
  @ complete behaviors;
  @ disjoint behaviors;
  @ assigns \\nothing;
  @*/
int find(int *arr, int n, int x);
