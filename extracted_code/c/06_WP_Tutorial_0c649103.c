/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_WP_Tutorial.md
 * Line: 715
 * Language: c
 * Block ID: 0c649103
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 排序后性质
/*@
  @requires n >= 0;
  @requires \valid(a + (0..n-1));
  @assigns a[0..n-1];
  @ensures is_sorted(a, n);
  @ensures permutation{Pre,Here}(a, n);  // 是原数组的排列
*/
void sort(int* a, int n);
