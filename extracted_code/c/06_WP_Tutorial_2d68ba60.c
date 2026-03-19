/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_WP_Tutorial.md
 * Line: 945
 * Language: c
 * Block ID: 2d68ba60
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 常见问题
/*@
  @loop invariant 0 <= i <= n;  // 太弱!
*/

// 解决方案: 添加功能性不变量
/*@
  @loop invariant 0 <= i <= n;
  @loop invariant sum == \sum(0,i-1,\lambda integer k; arr[k]);
  @loop invariant \forall integer j; 0 <= j < i ==> processed[j];
*/
