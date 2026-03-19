/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_WP_Tutorial.md
 * Line: 275
 * Language: c
 * Block ID: 86ce3f46
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  // 定义谓词
  @predicate is_sorted(int* a, integer n) =
    \forall integer i, j; 0 <= i <= j < n ==> a[i] <= a[j];

  // 定义逻辑函数
  @logic integer sum(int* a, integer n) reads a[0..n-1];

  // 函数公理化
  @axiom sum_empty:
    \forall int* a; sum(a, 0) == 0;

  @axiom sum_step:
    \forall int* a, integer n; n > 0 ==>
      sum(a, n) == sum(a, n-1) + a[n-1];

  // 引理 (需要证明)
  @lemma sum_nonneg:
    \forall int* a, integer n;
      n >= 0 ==> (\forall integer i; 0 <= i < n ==> a[i] >= 0) ==>
      sum(a, n) >= 0;
*/

/*@
  @requires n >= 0;
  @requires \valid_read(arr + (0..n-1));
  @requires \forall integer i; 0 <= i < n ==> arr[i] >= 0;
  @assigns \nothing;
  @ensures \result == sum(arr, n);
  @ensures \result >= 0;
*/
int sum_positive(const int* arr, int n);
