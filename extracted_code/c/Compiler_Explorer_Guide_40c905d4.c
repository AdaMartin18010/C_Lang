/*
 * Auto-generated from: 14_Video_Tutorials\02_Compiler_Explorer_Integration\Compiler_Explorer_Guide.md
 * Line: 433
 * Language: c
 * Block ID: 40c905d4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 非尾递归
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

// 尾递归
int factorial_tail(int n, int acc) {
    if (n <= 1) return acc;
    return factorial_tail(n - 1, n * acc);
}
