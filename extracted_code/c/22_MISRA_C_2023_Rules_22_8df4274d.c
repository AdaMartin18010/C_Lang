/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\22_MISRA_C_2023_Rules_22.md
 * Line: 57
 * Language: c
 * Block ID: 8df4274d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 使用或移除 */
void func(void)
{
    int used = 10;
    printf("%d\n", used);
}

/* ✅ 合规 - 条件编译 */
#ifdef DEBUG
static void debug_helper(void)
{
    /* ... */
}
#endif

/* ✅ 合规 - 显式抑制警告 */
void callback(void *unused_param)
{
    (void)unused_param;  /* 明确不使用 */
    /* ... */
}
