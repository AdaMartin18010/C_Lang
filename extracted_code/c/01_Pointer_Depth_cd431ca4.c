/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Depth.md
 * Line: 1405
 * Language: c
 * Block ID: cd431ca4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ UNSAFE: 野指针 */
void wild_pointer_example(void) {
    int *p;           /* 未初始化 - 野指针 */
    *p = 10;          /* UB: 可能写入任意地址！ */
}

/* ❌ UNSAFE: 条件路径中可能未初始化 */
int *maybe_wild(int condition) {
    int *p;
    if (condition) {
        p = malloc(sizeof(int));
    }
    /* 如果 condition 为假，p 仍是野指针 */
    return p;  /* 可能返回野指针 */
}

/* ✅ SAFE: 始终初始化 */
void safe_init(void) {
    int *p = NULL;    /* 初始化为空指针 */
    /* ... */
    if (p != NULL) {  /* 安全检查 */
        *p = 10;
    }
}
