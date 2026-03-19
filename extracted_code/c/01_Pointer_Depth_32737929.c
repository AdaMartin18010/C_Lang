/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Depth.md
 * Line: 1491
 * Language: c
 * Block ID: 32737929
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ UNSAFE: 数组越界 */
void buffer_overflow(int *arr, int n) {
    for (int i = 0; i <= n; i++) {  /* BUG: 应该是 < n */
        arr[i] = 0;                 /* 当i==n时越界 */
    }
}

/* ❌ UNSAFE: 指针算术越界 */
void pointer_arith_oob(void) {
    int arr[10];
    int *end = arr + 10;     /* 尾后指针是有效的 */
    *end = 0;                /* UB: 解引用尾后指针！ */

    int *far = arr + 100;    /* 严重越界 */
    *far = 0;                /* 可能崩溃或破坏其他数据 */
}

/* ❌ UNSAFE: 字符串操作越界 */
void string_overflow(void) {
    char buf[10];
    strcpy(buf, "Hello World!");  /* 12字符 > 10字节 */
}

/* ✅ SAFE: 边界检查 */
void safe_access(int *arr, size_t n, size_t index) {
    if (arr == NULL || index >= n) {
        return;  /* 或报告错误 */
    }
    arr[index] = 0;
}

/* ✅ SAFE: 使用安全函数 */
void safe_string_op(void) {
    char buf[10];
    strncpy(buf, "Hello World!", sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';  /* 确保终止 */
}
