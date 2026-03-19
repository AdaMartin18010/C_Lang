/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\C23_Standard_Library\02_stdckdint_h_Complete_Reference.md
 * Line: 52
 * Language: c
 * Block ID: 25e1ef7c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 方法 1: 传统手动检查 (容易出错)
int unsafe_add(int a, int b) {
    if (a > 0 && b > INT_MAX - a) {
        // overflow
    }
    return a + b;
}

// 方法 2: 内置函数 (GCC/Clang 特定)
int gcc_add(int a, int b) {
    int result;
    bool overflow = __builtin_add_overflow(a, b, &result);
    // ...
}

// 方法 3: C23 标准方法 (可移植)
int c23_add(int a, int b) {
    int result;
    bool overflow = ckd_add(&result, a, b);
    // ...
}
