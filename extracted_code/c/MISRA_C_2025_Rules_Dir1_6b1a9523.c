/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir1.md
 * Line: 94
 * Language: c
 * Block ID: 6b1a9523
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 [C:2023延续] - 使用GCC嵌套函数扩展（非标准C） */
void foo(void)
{
    void bar(void) {  /* GCC扩展 */
        printf("nested\n");
    }
    bar();
}

/* ❌ 违反 [C:2023延续] - 超过函数参数限制 */
void func(int a0, ..., int a127, int a128);  /* 128个参数 */

/* ❌ 违反 [C23相关] - 使用编译器不支持C23特性 */
#if __STDC_VERSION__ < 202311L
/* 在不支持C23的编译器上使用C23特性 */
constexpr int x = 5;  /* 编译错误 */
#endif
