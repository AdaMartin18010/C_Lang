/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\02_MISRA_C_2023_Rules_2.md
 * Line: 262
 * Language: c
 * Block ID: c69d937a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 Rule 2.2 */
void check(int x)
{
    x > 10;  /* 比较结果未使用！ */

    if (x > 10) {  /* 正确的写法 */
        process();
    }
}
