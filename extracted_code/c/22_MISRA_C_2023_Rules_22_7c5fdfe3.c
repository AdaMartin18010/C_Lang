/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\22_MISRA_C_2023_Rules_22.md
 * Line: 121
 * Language: c
 * Block ID: 7c5fdfe3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 无死代码 */
int func(void)
{
    printf("Doing work\n");
    return 0;
}

/* ✅ 合规 - 条件编译 */
#ifdef NEVER_ENABLED
void unused_func(void)
{
    /* 整个函数条件编译 */
}
#endif
