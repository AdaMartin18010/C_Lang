/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\06_MISRA_C_2023_Rules_6.md
 * Line: 366
 * Language: c
 * Block ID: b8fca6dc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 无死代码 */
void func(void)
{
    int x = 0;
    /* 使用x */
    return;
}

/* ✅ 合规 - 使用条件编译 */
#ifdef NEVER_ENABLED
void unused_func(void)  /* 整个函数被条件编译 */
{
    /* ... */
}
#endif
