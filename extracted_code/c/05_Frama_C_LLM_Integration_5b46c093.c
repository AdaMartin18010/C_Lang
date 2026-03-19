/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Frama_C_LLM_Integration.md
 * Line: 480
 * Language: c
 * Block ID: 5b46c093
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================
 * 示例：数学函数库的LLM辅助验证
 * 文件名: math_lib.c
 * ============================================================ */

/*@
  @ axiomatic MathProperties {
  @   axiom square_nonnegative:
  @     \forall real x; x*x >= 0;
  @ }
  @*/

/* ============================================================
 * 示例1: 整数平方根
 * ============================================================ */

// 原始代码（无规范）
int isqrt(int n) {
    if (n < 0) return -1;
    if (n == 0 || n == 1) return n;

    int x = n;
    int y = (x + 1) / 2;

    while (y < x) {
        x = y;
        y = (x + n / x) / 2;
    }

    return x;
}

// LLM生成的完整ACSL规范
/*@
  @ requires n >= 0;
  @ ensures \result >= 0;
  @ ensures \result * \result <= n;
  @ ensures (\result + 1) * (\result + 1) > n || \result == 65535;
  @ assigns \nothing;
  @ behavior negative:
  @   assumes n < 0;
  @   ensures \result == -1;
  @ behavior non_negative:
  @   assumes n >= 0;
  @   ensures \result * \result <= n < (\result + 1) * (\result + 1);
  @ complete behaviors;
  @ disjoint behaviors;
  @*/
int isqrt_annotated(int n);

// 添加循环不变量的完整实现
/*@
  @ requires n >= 0;
  @ ensures \result >= 0;
  @ ensures \result * \result <= n;
  @ ensures (\result + 1) * (\result + 1) > n;
  @ assigns \nothing;
  @*/
int isqrt_proved(int n) {
    if (n == 0 || n == 1) return n;

    int x = n;
    int y = (x + 1) / 2;

    /*@
      @ loop invariant x > 0 && y > 0;
      @ loop invariant x * x >= n;
      @ loop invariant y == (x + n / x) / 2;
      @ loop invariant y <= x;
      @ loop variant x;
      @*/
    while (y < x) {
        x = y;
        y = (x + n / x) / 2;
    }

    return x;
}

/* ============================================================
 * 示例2: 数组求和
 * ============================================================ */

/*@
  @ requires n >= 0;
  @ requires \valid(arr+(0..n-1));
  @ ensures \result == \sum(0, n-1, \lambda integer i; arr[i]);
  @ assigns \nothing;
  @ behavior empty:
  @   assumes n == 0;
  @   ensures \result == 0;
  @*/
int array_sum(int *arr, int n) {
    int sum = 0;

    /*@
      @ loop invariant 0 <= i <= n;
      @ loop invariant sum == \sum(0, i-1, \lambda integer j; arr[j]);
      @ loop assigns i, sum;
      @ loop variant n - i;
      @*/
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }

    return sum;
}

/* ============================================================
 * 示例3: 安全的内存拷贝
 * ============================================================ */

/*@
  @ predicate disjoint_or_equal{L}(char *d, char *s, integer n) =
  @   \separated(d, s) || (d == s);
  @*/

/*@
  @ requires n >= 0;
  @ requires \valid(dest+(0..n-1));
  @ requires \valid(src+(0..n-1));
  @ requires disjoint_or_equal(dest, src, n);
  @ ensures \forall integer i; 0 <= i < n ==> dest[i] == \old(src[i]);
  @ assigns dest[0..n-1];
  @*/
void safe_memcpy(char *dest, const char *src, int n) {
    /*@
      @ loop invariant 0 <= i <= n;
      @ loop invariant \forall integer j; 0 <= j < i ==> dest[j] == \old(src[j]);
      @ loop assigns i, dest[0..n-1];
      @ loop variant n - i;
      @*/
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

/* ============================================================
 * 示例4: 查找最大子数组和 (Kadane算法)
 * ============================================================ */

/*@
  @ requires n >= 0;
  @ requires \valid(arr+(0..n-1));
  @ ensures \forall integer i, j; 0 <= i <= j < n ==>
  @         \sum(i, j, \lambda integer k; arr[k]) <= \result;
  @ ensures n > 0 ==>
  @         \exists integer i, j; 0 <= i <= j < n &&
  @         \sum(i, j, \lambda integer k; arr[k]) == \result;
  @ assigns \nothing;
  @*/
int max_subarray_sum(int *arr, int n) {
    if (n == 0) return 0;

    int max_ending_here = arr[0];
    int max_so_far = arr[0];

    /*@
      @ loop invariant 1 <= i <= n;
      @ loop invariant max_ending_here ==
      @   \max(0, i-1, \lambda integer k;
      @     \sum(k, i-1, \lambda integer m; arr[m]));
      @ loop invariant max_so_far ==
      @   \max(0, i-1, \lambda integer k;
      @     \max(k, i-1, \lambda integer m;
      @       \sum(k, m, \lambda integer p; arr[p])));
      @ loop assigns i, max_ending_here, max_so_far;
      @ loop variant n - i;
      @*/
    for (int i = 1; i < n; i++) {
        if (max_ending_here + arr[i] > arr[i])
            max_ending_here = max_ending_here + arr[i];
        else
            max_ending_here = arr[i];

        if (max_ending_here > max_so_far)
            max_so_far = max_ending_here;
    }

    return max_so_far;
}
