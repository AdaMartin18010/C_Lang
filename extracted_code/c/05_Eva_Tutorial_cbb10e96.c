/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Eva_Tutorial.md
 * Line: 1036
 * Language: c
 * Block ID: cbb10e96
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 解决方案: 添加循环不变量辅助Eva
/*@
  @loop invariant 0 <= i <= n;
  @loop invariant \forall integer j; 0 <= j < i ==> arr[j] == \old(arr[j]);
  @loop assigns i, arr[0..n-1];
  @loop variant n - i;
*/
for (int i = 0; i < n; i++) { ... }
