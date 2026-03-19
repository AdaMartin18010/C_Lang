/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\04_Dynamic_Programming_Basics.md
 * Line: 216
 * Language: c
 * Block ID: b1a987f1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 动态规划经典问题
 * 文件名: dp_classic.c
 * 编译: gcc -std=c23 -O2 -o dp_classic dp_classic.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// ==================== 斐波那契数列 ====================

/**
 * @brief 递归版本 - 时间O(2^n)，存在大量重复计算
 */
unsigned long long fib_recursive(int n) {
    if (n <= 1) return n;
    return fib_recursive(n - 1) + fib_recursive(n - 2);
}

/**
 * @brief 记忆化搜索版本 - 时间O(n)，空间O(n)
 */
unsigned long long fib_memoization(int n, unsigned long long memo[]) {
    if (n <= 1) return n;
    if (memo[n] != 0) return memo[n];

    memo[n] = fib_memoization(n - 1, memo) + fib_memoization(n - 2, memo);
    return memo[n];
}

unsigned long long fib_memo(int n) {
    unsigned long long *memo = calloc(n + 1, sizeof(unsigned long long));
    unsigned long long result = fib_memoization(n, memo);
    free(memo);
    return result;
}

/**
 * @brief 自底向上DP - 时间O(n)，空间O(n)
 */
unsigned long long fib_dp(int n) {
    if (n <= 1) return n;

    unsigned long long *dp = malloc((n + 1) * sizeof(unsigned long long));
    dp[0] = 0;
    dp[1] = 1;

    for (int i = 2; i <= n; i++) {
        dp[i] = dp[i - 1] + dp[i - 2];
    }

    unsigned long long result = dp[n];
    free(dp);
    return result;
}

/**
 * @brief 空间优化版本 - 时间O(n)，空间O(1)
 */
unsigned long long fib_optimized(int n) {
    if (n <= 1) return n;

    unsigned long long prev2 = 0;  // F(n-2)
    unsigned long long prev1 = 1;  // F(n-1)
    unsigned long long current;

    for (int i = 2; i <= n; i++) {
        current = prev1 + prev2;
        prev2 = prev1;
        prev1 = current;
    }

    return prev1;
}

/**
 * @brief 矩阵快速幂版本 - 时间O(log n)
 *
 * [F(n)  ]   [1 1]   [F(n-1)]
 * [F(n-1)] = [1 0] * [F(n-2)]
 */
typedef struct {
    unsigned long long a, b, c, d;
} Matrix2x2;

Matrix2x2 matrix_mult(Matrix2x2 m1, Matrix2x2 m2) {
    Matrix2x2 result;
    result.a = m1.a * m2.a + m1.b * m2.c;
    result.b = m1.a * m2.b + m1.b * m2.d;
    result.c = m1.c * m2.a + m1.d * m2.c;
    result.d = m1.c * m2.b + m1.d * m2.d;
    return result;
}

Matrix2x2 matrix_pow(Matrix2x2 base, int n) {
    if (n == 1) return base;

    if (n % 2 == 0) {
        Matrix2x2 half = matrix_pow(base, n / 2);
        return matrix_mult(half, half);
    } else {
        return matrix_mult(base, matrix_pow(base, n - 1));
    }
}

unsigned long long fib_matrix(int n) {
    if (n <= 1) return n;

    Matrix2x2 base = {1, 1, 1, 0};
    Matrix2x2 result = matrix_pow(base, n);

    return result.b;  // F(n) = M^n 的 (0,1) 或 (1,0) 位置
}
