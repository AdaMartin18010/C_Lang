/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\06_MISRA_C_2023_Rules_6.md
 * Line: 668
 * Language: c
 * Block ID: 21e2c21d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 单独空语句 */
;  /* 无意义的空语句 */

/* ❌ 违反 - 循环体为空 */
while (*ptr++);  /* 难以发现的分号 */

/* ❌ 违反 - if后空语句 */
if (condition);
{
    /* 这不在if内！ */
    action();
}
