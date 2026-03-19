/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\10_MISRA_C_2023_Rules_10.md
 * Line: 364
 * Language: c
 * Block ID: 4c4498d5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 不一致的声明 */
/* file1.c */
int shared_var = 0;  /* 定义 */

/* file2.c */
extern int shared_var;  /* 声明 - 这是正确的 */
extern int shared_var = 0;  /* 错误：extern带初始化 */

/* ❌ 违反 - static与extern冲突 */
static int internal = 0;  /* 内部链接 */

void func(void)
{
    extern int internal;  /* 试图声明外部链接 */
}

/* ❌ 违反 - 隐式extern */
int global;  /* 隐式extern，不清晰 */
