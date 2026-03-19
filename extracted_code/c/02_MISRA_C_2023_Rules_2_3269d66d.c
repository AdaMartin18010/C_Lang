/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\02_MISRA_C_2023_Rules_2.md
 * Line: 250
 * Language: c
 * Block ID: 3269d66d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 Rule 2.2 */
void count(void)
{
    int i = 0;
    i++;  /* i++的结果未使用，但i本身被修改 */
    /* 如果意图是修改i，应该使用i += 1 */
}
