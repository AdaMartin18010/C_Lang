/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\06_MISRA_C_2023_Rules_6.md
 * Line: 218
 * Language: c
 * Block ID: 2799600f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 整数作为布尔 */
int flag = 1;
if (flag) {  /* 虽然有效，但不明确 */
    /* ... */
}

/* ❌ 违反 - 隐式布尔转换 */
int x = 5;
if (x) {  /* 不清晰：是检查非零还是真值？ */
    /* ... */
}

/* ❌ 违反 - 布尔运算混淆 */
bool a = true;
bool b = false;
bool c = a & b;  /* 位运算，不是逻辑运算 */
