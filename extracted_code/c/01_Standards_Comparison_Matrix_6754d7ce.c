/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 2511
 * Language: c
 * Block ID: 6754d7ce
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 可能的C26 constexpr扩展 */
constexpr int* allocate_array(size_t n) {
    /* 编译期内存分配 */
    static int buffer[1000];
    return buffer;
}

constexpr int fibonacci(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        int temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

constexpr int fib_50 = fibonacci(50);  /* 编译期计算 */
