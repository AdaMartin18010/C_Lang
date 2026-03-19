/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Frama_C_LLM_Integration.md
 * Line: 1131
 * Language: c
 * Block ID: d34918b0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================
 * 示例：LLM辅助证明冒泡排序
 * 文件: bubble_sort_llm.c
 * ============================================================ */

/*@
  @ predicate sorted{L}(int *a, integer n) =
  @   \forall integer i, j; 0 <= i <= j < n ==> a[i] <= a[j];
  @
  @ predicate permutation{L1,L2}(int *a, int *b, integer n) =
  @   \forall integer v;
  @     \numof(0, n-1, \lambda integer i; a[i] == v) ==
  @     \numof(0, n-1, \lambda integer i; b[i] == v);
  @
  @ predicate swap{L}(int *a, integer i, integer j) =
  @   a[i] == \old(a[j]) && a[j] == \old(a[i]);
  @*/

/* 交换两个元素 */
/*@
  @ requires \valid(a+i) && \valid(a+j);
  @ ensures swap(a, i, j);
  @ assigns a[i], a[j];
  @*/
void swap(int *a, int i, int j) {
    int tmp = a[i];
    a[i] = a[j];
    a[j] = tmp;
}

/* ============================================================
 * 第一轮：LLM生成的基础规范
 * ============================================================ */

/* LLM生成的初始规范（可能需要迭代改进） */
/*@
  @ requires n >= 0;
  @ requires \valid(a+(0..n-1));
  @ ensures sorted(a, n);
  @ ensures permutation{Pre,Post}(a, a, n);
  @ assigns a[0..n-1];
  @*/
void bubble_sort_v1(int *a, int n);

/* ============================================================
 * 第二轮：添加详细的循环不变量
 * ============================================================ */

/*@
  @ requires n >= 0;
  @ requires \valid(a+(0..n-1));
  @ ensures sorted(a, n);
  @ ensures permutation{Pre,Post}(a, a, n);
  @ assigns a[0..n-1];
  @*/
void bubble_sort_v2(int *a, int n) {
    /*@
      @ loop invariant 0 <= i <= n;
      @ loop invariant sorted(a, n-i, n);  // 尾部已排序
      @ loop invariant permutation{Pre,Here}(a, a, n);
      @ loop assigns i, a[0..n-1];
      @ loop variant n - i;
      @*/
    for (int i = 0; i < n; i++) {
        /*@
          @ loop invariant 0 <= j <= n - i - 1;
          @ loop invariant \forall integer k; 0 <= k < j ==> a[k] <= a[j];
          @ loop invariant permutation{Pre,Here}(a, a, n);
          @ loop assigns j, a[0..n-1];
          @ loop variant n - i - 1 - j;
          @*/
        for (int j = 0; j < n - i - 1; j++) {
            if (a[j] > a[j + 1]) {
                swap(a, j, j + 1);
            }
        }
    }
}

/* ============================================================
 * 第三轮：完整的、可验证的实现
 * ============================================================ */

/*@
  @ lemma swap_preserves_sorted_tail:
  @   \forall int *a, integer n, i, j;
  @     0 <= i < j < n ==>
  @     sorted(a, j+1, n) ==>
  @     swap(a, i, i+1) ==>
  @     sorted(a, j+1, n);
  @
  @ lemma bubble_step_preserves_partial_sorted:
  @   \forall int *a, integer n, j;
  @     0 <= j < n-1 ==>
  @     (a[j] > a[j+1] ==>
  @       (swap(a,j,j+1) ==>
  @         \forall integer k; 0 <= k < j+1 ==> a[k] <= a[j+1]));
  @*/

/*@
  @ requires n >= 0;
  @ requires \valid(a+(0..n-1));
  @ ensures sorted(a, n);
  @ ensures permutation{Pre,Post}(a, a, n);
  @ assigns a[0..n-1];
  @*/
void bubble_sort_final(int *a, int n) {
    if (n <= 1) return;

    /*@
      @ loop invariant 0 <= i < n;
      @ loop invariant sorted(a, n-i, n);
      @ // 外层循环结束后，a[n-i..n-1]是有序的
      @ loop invariant i > 0 ==> a[n-i] >= a[n-i-1];
      @ loop invariant permutation{Pre,Here}(a, a, n);
      @ loop assigns i, j, a[0..n-1];
      @ loop variant n - i;
      @*/
    for (int i = 0; i < n - 1; i++) {

        /*@ ghost int old_a_n_i = a[n-i-1]; */

        /*@
          @ loop invariant 0 <= j < n - i;
          @ loop invariant \forall integer k;
          @   0 <= k < j ==> a[k] <= a[j];
          @ // 当前扫描的最大值在a[j]
          @ loop invariant j > 0 ==> a[j] >= a[j-1];
          @ loop invariant permutation{LoopEntry,Here}(a, a, n);
          @ loop assigns j, a[0..n-i-1];
          @ loop variant n - i - 1 - j;
          @*/
        for (int j = 0; j < n - i - 1; j++) {

            /*@
              @ assert \valid(a+j) && \valid(a+j+1);
              @*/

            if (a[j] > a[j + 1]) {
                /*@
                  @ assert a[j] > a[j+1];
                  @ assert a[j+1] < a[j];
                  @*/
                swap(a, j, j + 1);
                /*@
                  @ assert a[j] < a[j+1];
                  @*/
            }

            /*@
              @ assert a[j] <= a[j+1];
              @ assert \forall integer k; 0 <= k <= j ==> a[k] <= a[j+1];
              @*/
        }

        /*@
          @ assert sorted(a, n-i-1, n);
          @ assert a[n-i-1] >= a[n-i-2];
          @*/
    }

    /*@
      @ assert sorted(a, n);
      @*/
}

/* ============================================================
 * LLM提示示例：如何请求改进证明
 * ============================================================

提示：
---
我尝试验证冒泡排序，但WP报告外层循环不变量失败。

当前代码：
