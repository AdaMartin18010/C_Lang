/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\07_MISRA_C_2023_Rules_7.md
 * Line: 429
 * Language: c
 * Block ID: 02fc89e6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 内存泄漏 */
void leak_example(void)
{
    int *p = malloc(100);
    /* 使用p */
    /* 忘记free(p)！ */
}

/* ❌ 违反 - 过早返回导致泄漏 */
void early_return(int condition)
{
    int *p = malloc(100);
    if (condition) {
        return;  /* 泄漏！ */
    }
    free(p);
}

/* ❌ 违反 - 异常路径泄漏 */
void exception_path(void)
{
    int *p = malloc(100);
    if (!init()) {
        return;  /* 泄漏！ */
    }
    if (!process()) {
        return;  /* 泄漏！ */
    }
    free(p);
}

/* ❌ 违反 - 双重释放 */
void double_free(void)
{
    int *p = malloc(100);
    free(p);
    free(p);  /* 双重释放！ */
}
