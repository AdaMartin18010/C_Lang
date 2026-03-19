/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Depth.md
 * Line: 1777
 * Language: c
 * Block ID: 9995a233
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ UNSAFE: 双重释放 */
void double_free_simple(void) {
    int *p = malloc(sizeof(int));
    free(p);
    free(p);  /* UB: 双重释放！ */
}

/* ❌ UNSAFE: 别名导致双重释放 */
void double_free_alias(void) {
    int *p = malloc(sizeof(int));
    int *q = p;  /* q 是 p 的别名 */

    free(p);
    free(q);     /* UB: 同一块内存被释放两次！ */
}

/* ❌ UNSAFE: 复杂场景 */
void double_free_complex(void) {
    int *arr[10];
    for (int i = 0; i < 10; i++) {
        arr[i] = malloc(sizeof(int));
    }

    arr[5] = arr[3];  /* 复制指针，现在有两个指向同一内存 */

    for (int i = 0; i < 10; i++) {
        free(arr[i]);  /* 当 i=3 和 i=5 时双重释放！ */
    }
}

/* ✅ SAFE: 释放后置NULL */
void safe_free(void **pp) {
    if (pp && *pp) {
        free(*pp);
        *pp = NULL;
    }
}

/* ✅ SAFE: 明确所有权转移 */
void ownership_transfer(void) {
    int *owner = malloc(sizeof(int));
    int *borrower = owner;  /* borrower 只是借用 */

    /* ... 使用 borrower ... */
    borrower = NULL;  /* 表明不再使用 */

    /* 只有 owner 释放 */
    safe_free((void**)&owner);
}
