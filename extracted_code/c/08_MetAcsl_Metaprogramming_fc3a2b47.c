/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\08_MetAcsl_Metaprogramming.md
 * Line: 194
 * Language: c
 * Block ID: fc3a2b47
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// metacsl_basic.c

// 应用自动生成规范
/*@ @meta auto_contract */
int simple_function(int x, int* p) {
    *p = x * 2;
    return x + 1;
}

// MetAcsl自动生成:
/*@
  @requires \valid(p);                    // 指针有效性
  @assigns *p;                            // 修改声明
  @ensures *p == x * 2;                   // 后置条件(推断)
  @ensures \result == x + 1;               // 返回值
*/
