/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Depth.md
 * Line: 1443
 * Language: c
 * Block ID: 7119330b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ UNSAFE: 释放后使用 (Use-After-Free) */
void use_after_free(void) {
    int *p = malloc(sizeof(int));
    *p = 42;
    free(p);
    /* p 现在是悬挂指针 */
    printf("%d\n", *p);  /* UB: 访问已释放内存 */

    /* 更危险的场景 */
    if (p != NULL) {     /* 检查无效！free不置NULL */
        *p = 100;        /* UB! 破坏堆结构 */
    }
}

/* ❌ UNSAFE: 返回局部变量地址 */
int *get_local(void) {
    int local = 42;
    return &local;       /* 函数返回后 local 不存在 */
}

/* ✅ SAFE: 正确做法 */
void safe_free(void **pp) {
    if (pp && *pp) {
        free(*pp);
        *pp = NULL;      /* 置NULL防止悬挂 */
    }
}

/* ✅ SAFE: 返回堆内存 */
int *get_heap_value(void) {
    int *p = malloc(sizeof(int));
    if (p) *p = 42;
    return p;            /* 调用者负责释放 */
}
