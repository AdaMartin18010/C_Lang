/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Eva_Tutorial.md
 * Line: 526
 * Language: c
 * Block ID: b494ab16
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// tutorial5_overflow.c
#include <limits.h>

// 有溢出风险的函数
int risky_add(int a, int b) {
    return a + b;  // 可能溢出!
}

// 安全的加法实现
/*@
  @requires a > 0 ==> b <= INT_MAX - a;
  @requires a < 0 ==> b >= INT_MIN - a;
  @assigns \nothing;
  @ensures \result == a + b;
*/
int safe_add(int a, int b) {
    return a + b;
}

// 使用内建函数检查
int checked_add(int a, int b, int* overflow) {
    if (a > 0 && b > INT_MAX - a) {
        *overflow = 1;
        return INT_MAX;
    }
    if (a < 0 && b < INT_MIN - a) {
        *overflow = 1;
        return INT_MIN;
    }
    *overflow = 0;
    return a + b;
}
