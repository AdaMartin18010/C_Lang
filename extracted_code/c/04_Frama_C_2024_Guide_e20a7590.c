/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\04_Frama_C_2024_Guide.md
 * Line: 879
 * Language: c
 * Block ID: e20a7590
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 常见问题: 不变量不够强

// 问题代码:
/*@
  @loop invariant 0 <= i <= n;  // 太弱!
*/
for (int i = 0; i < n; i++) { ... }

// 修正: 添加功能性不变量
/*@
  @loop invariant 0 <= i <= n;
  @loop invariant sum == \sum(0,i-1,\lambda integer k; arr[k]);
  @loop assigns i, sum;
  @loop variant n - i;
*/
for (int i = 0; i < n; i++) { sum += arr[i]; }
