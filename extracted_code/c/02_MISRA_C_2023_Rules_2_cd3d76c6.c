/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\02_MISRA_C_2023_Rules_2.md
 * Line: 95
 * Language: c
 * Block ID: cd3d76c6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 Rule 2.1 - return后的代码不可达 */
int calculate(int x)
{
    if (x < 0) {
        return -1;
    }
    return x * 2;
    printf("Result calculated\n");  /* 不可达！ */
}
