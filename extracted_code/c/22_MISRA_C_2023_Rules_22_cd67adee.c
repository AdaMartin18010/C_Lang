/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\22_MISRA_C_2023_Rules_22.md
 * Line: 29
 * Language: c
 * Block ID: cd67adee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 未使用变量 */
void func(void)
{
    int unused = 10;  /* 从未使用 */
    printf("Hello\n");
}

/* ❌ 违反 - 未使用函数 */
static void helper(void)  /* 只在当前文件 */
{
    /* ... */
}
/* 从未调用 */

/* ❌ 违反 - 未使用宏 */
#define UNUSED_MACRO 100

/* ❌ 违反 - 未使用标签 */
void func(void)
{
unused_label:
    printf("Hello\n");
}
