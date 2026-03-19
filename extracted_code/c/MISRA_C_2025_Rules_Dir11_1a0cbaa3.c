/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 232
 * Language: c
 * Block ID: 1a0cbaa3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 防止野指针（悬空指针） */

/* ❌ 违反 [待官方确认] - 返回局部变量地址 */
int *bad_func(void) {
    int local = 5;
    return &local;  /* 返回后local不存在 */
}

/* ❌ 违反 [待官方确认] - 使用已释放内存 */
int *p = malloc(sizeof(int));
free(p);
*p = 5;  /* 使用已释放内存（UB） */

/* ✅ 合规 [待官方确认] */
int *safe_alloc(void) {
    int *p = malloc(sizeof(int));
    return p;  /* 堆内存有效 */
}

void safe_use(void) {
    int *p = safe_alloc();
    if (p != NULL) {
        *p = 5;
        free(p);
        p = NULL;  /* 置空防止误用 */
    }
}
