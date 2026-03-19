/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\00_Quick_Reference_Guide.md
 * Line: 321
 * Language: c
 * Block ID: 0fb9bf7c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 错误 */
void leak(void)
{
    int *p = malloc(100);
    /* 使用p */
    /* 忘记free */
}

/* ✅ 修复 */
void no_leak(void)
{
    int *p = malloc(100);
    if (p == NULL) {
        return;
    }
    /* 使用p */
    free(p);
}
