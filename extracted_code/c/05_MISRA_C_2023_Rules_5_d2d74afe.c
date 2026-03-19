/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\05_MISRA_C_2023_Rules_5.md
 * Line: 220
 * Language: c
 * Block ID: d2d74afe
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 指针转float */
int data = 42;
float f = (float)&data;  /* 无意义！ */

/* ❌ 违反 - 指针转结构体 */
struct weird {
    int *ptr;
};
int x;
struct weird w = (struct weird)&x;  /* 危险！ */

/* ❌ 违反 - 不安全的函数指针转换 */
int func(int x) { return x; }
void (*bad_ptr)(void) = (void (*)(void))func;  /* 类型不匹配 */
bad_ptr();  /* 调用约定可能不同！ */
