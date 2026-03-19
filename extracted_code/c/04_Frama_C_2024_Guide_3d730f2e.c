/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\04_Frama_C_2024_Guide.md
 * Line: 900
 * Language: c
 * Block ID: 3d730f2e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 问题: 重叠数组导致验证失败

// 解决方案: 显式声明分离
/*@
  @requires \separated(dst+(0..n-1), src+(0..n-1));
*/
void safe_copy(int* dst, const int* src, int n);

// 或者使用行为分支
/*@
  @behavior separate:
    @assumes \separated(dst+(0..n-1), src+(0..n-1));
    @ensures ...
  @behavior overlap:
    @assumes !\separated(dst+(0..n-1), src+(0..n-1));
    @ensures ...
*/
