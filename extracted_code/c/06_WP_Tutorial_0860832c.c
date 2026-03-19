/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_WP_Tutorial.md
 * Line: 701
 * Language: c
 * Block ID: 0860832c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 非负性证明
/*@
  @requires x >= 0 && y >= 0;
  @ensures \result >= 0;
*/
int sum_nonneg(int x, int y) {
    return x + y;
}
// 证明策略: Alt-Ergo自动完成
