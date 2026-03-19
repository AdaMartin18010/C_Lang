/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\07_MISRA_C_2023_Rules_7.md
 * Line: 215
 * Language: c
 * Block ID: 2ae877e1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 释放后置空 */
int *p = malloc(sizeof(int));
if (p != NULL) {
    *p = 42;
    free(p);
    p = NULL;  /* 置空 */
}
/* 后续使用会崩溃（可检测）而非未定义行为 */

/* ✅ 合规 - 双重释放保护 */
void safe_free(void **pp)
{
    if (pp != NULL && *pp != NULL) {
        free(*pp);
        *pp = NULL;
    }
}

/* 使用 */
safe_free((void **)&p);
safe_free((void **)&p);  /* 安全，无效果 */

/* ✅ 合规 - 动态分配替代局部变量 */
int *create_value(int val)
{
    int *p = malloc(sizeof(int));
    if (p != NULL) {
        *p = val;
    }
    return p;  /* 返回堆内存 */
}

/* ✅ 合规 - 静态变量（谨慎使用）*/
int *get_static(void)
{
    static int value = 42;  /* 静态生命周期 */
    return &value;  /* 安全 */
}
