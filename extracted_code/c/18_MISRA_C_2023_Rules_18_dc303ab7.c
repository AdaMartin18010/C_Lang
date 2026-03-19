/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\18_MISRA_C_2023_Rules_18.md
 * Line: 296
 * Language: c
 * Block ID: dc303ab7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - VLA在栈上 */
void process(int n)
{
    int arr[n];  /* VLA - 栈溢出风险 */
    /* ... */
}

/* ❌ 违反 - VLA作为参数 */
void func(int n, int arr[n]);  /* VLA参数 */

/* ❌ 违反 - 二维VLA */
void matrix(int rows, int cols, int m[rows][cols]);
