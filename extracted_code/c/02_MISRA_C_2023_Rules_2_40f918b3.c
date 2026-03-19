/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\02_MISRA_C_2023_Rules_2.md
 * Line: 238
 * Language: c
 * Block ID: 40f918b3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 Rule 2.2 */
void process(void)
{
    int x = 10;
    x + 5;  /* 计算结果未使用，无效！ */
    x * 2;  /* 无效！ */
}
