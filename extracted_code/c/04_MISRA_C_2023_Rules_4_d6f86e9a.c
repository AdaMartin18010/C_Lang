/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\04_MISRA_C_2023_Rules_4.md
 * Line: 151
 * Language: c
 * Block ID: d6f86e9a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 局部变量隐藏全局变量 */
int debug_level = 0;  /* 全局 */

void set_debug_level(void)
{
    int debug_level = 1;  /* 隐藏全局变量 */
    /* 这里修改的是局部变量，不是全局变量！ */
}

/* ❌ 违反 - 参数隐藏全局变量 */
int timeout = 1000;  /* 全局默认超时 */

void init(int timeout)  /* 参数隐藏全局变量 */
{
    /* 内部使用timeout参数，全局timeout不可见 */
}

/* ❌ 违反 - 内部extern声明 */
static int var = 0;  /* 内部链接 */

void func(void)
{
    extern int var;  /* 试图声明外部链接，冲突！ */
}
