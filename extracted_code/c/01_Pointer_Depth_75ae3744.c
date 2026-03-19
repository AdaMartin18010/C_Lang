/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Depth.md
 * Line: 1702
 * Language: c
 * Block ID: 75ae3744
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ UNSAFE: 简单内存泄漏 */
void simple_leak(void) {
    int *p = malloc(sizeof(int));
    *p = 42;
    /* 函数返回，p 丢失，内存泄漏 */
}

/* ❌ UNSAFE: 条件路径泄漏 */
void conditional_leak(int condition) {
    int *p = malloc(sizeof(int));
    if (condition) {
        return;  /* 泄漏！ */
    }
    free(p);
}

/* ❌ UNSAFE: 重新赋值导致泄漏 */
void reassign_leak(void) {
    int *p = malloc(sizeof(int));
    p = malloc(sizeof(int));  /* 第一个分配丢失！ */
    free(p);
}

/* ❌ UNSAFE: 异常路径泄漏 */
int error_path_leak(int n) {
    int *arr = malloc(n * sizeof(int));
    if (n < 0) {
        return -1;  /* 泄漏！ */
    }
    /* 使用 arr... */
    free(arr);
    return 0;
}

/* ✅ SAFE: 正确清理 */
void no_leak(void) {
    int *p = malloc(sizeof(int));
    if (!p) return;

    *p = 42;
    /* ... 使用 p ... */

    free(p);
    p = NULL;
}

/* ✅ SAFE: 单一出口点 */
int single_exit(int n) {
    int *arr = malloc(n * sizeof(int));
    int result = -1;

    if (!arr) goto cleanup;
    if (n < 0) goto cleanup;

    /* ... 使用 arr ... */
    result = 0;

cleanup:
    free(arr);
    return result;
}
