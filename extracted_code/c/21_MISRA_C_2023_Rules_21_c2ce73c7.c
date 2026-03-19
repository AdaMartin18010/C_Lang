/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\21_MISRA_C_2023_Rules_21.md
 * Line: 28
 * Language: c
 * Block ID: c2ce73c7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 可能除以零 */
int divide(int a, int b)
{
    return a / b;  /* b可能为0 */
}

/* ❌ 违反 - 用户输入 */
int calculate(int x, int y)
{
    return 100 / (x - y);  /* x==y时除以零 */
}
