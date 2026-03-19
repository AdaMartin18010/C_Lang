/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\04_Frama_C_Integration.md
 * Line: 67
 * Language: c
 * Block ID: 35841da4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ACSL (ANSI C Specification Language) 语法示例

/*@
  // 函数合约
  requires \valid(p);           // 前置条件
  requires *p >= 0;
  ensures \result == *p + 1;     // 后置条件
  assigns \nothing;              // 不修改内存
*/
int increment(const int* p);

/*@
  // 循环不变式
  loop invariant 0 <= i <= n;
  loop invariant \forall integer j; 0 <= j < i ==> a[j] == b[j];
  loop assigns i, a[0..n-1];
  loop variant n - i;
*/
void array_copy(int* a, const int* b, int n) {
    for (int i = 0; i < n; i++) {
        a[i] = b[i];
    }
}

/*@
  // 内存谓词
  predicate is_valid_array(int* a, integer n) =
    \valid(a + (0..n-1));

  // 逻辑函数
  logic integer sum(int* a, integer n) reads a[0..n-1];
  axiom sum_empty: \forall int* a; sum(a, 0) == 0;
  axiom sum_step: \forall int* a, integer n; n > 0 ==>
    sum(a, n) == sum(a, n-1) + a[n-1];
*/
