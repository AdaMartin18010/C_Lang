/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\01_MISRA_C_2023_Rules_1-5.md
 * Line: 30
 * Language: c
 * Block ID: 2e4608ff
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 使用GCC嵌套函数扩展（非标准C） */
void foo(void)
{
    void bar(void) {  /* GCC扩展 */
        printf("nested\n");
    }
    bar();
}

/* ❌ 违反 - 超过127个函数参数 */
void func(int a0, ..., int a127, int a128);  /* 128个参数 */
